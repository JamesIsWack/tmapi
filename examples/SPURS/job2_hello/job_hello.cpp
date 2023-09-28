/*
*  SCE CONFIDENTIAL                                                         
*  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
*  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
*  All Rights Reserved.                                                     
*
*  Description: 
*  SN Systems modified version of the job_hello sample included in the SDK.
*
*  Demonstrates SPURs debugging using calls to snPause() in the job you wish
*  to debug
*               
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <cell/spurs.h>

extern char _binary_job_spurs_job_hello_bin_start[];	
extern char _binary_job_spurs_job_hello_bin_size[];
extern char _binary_job_spurs_job_send_event_bin_start[];	
extern char _binary_job_spurs_job_send_event_bin_size[];

CellSpursJob128 JobHello     __attribute__((__aligned__(16)));
CellSpursJob128 JobSendEvent __attribute__((__aligned__(16)));


typedef CellSpursJob128 JobType;

/* this cannot be less than 256 */
#define MAX_JOB_DESCRIPTOR_SIZE			(sizeof(JobType) > 256 ? sizeof(JobType) : 256)
#define SAMPLE_NAME						"sample_spurs_job_hello"

//Use a global var to specify job debugging.
#ifdef _DEBUG
uint8_t DEBUG_JOB_HELLO = 1;
#else
uint8_t DEBUG_JOB_HELLO = 0;
#endif 

static inline
uint32_t ceil16(uint32_t val)
{
	return (((val) + 15) & ~15);
}

static void 
jobHelloInit(CellSpursJob128 *job)
{
	__builtin_memset(job, 0, sizeof(CellSpursJob128));

	job->header.eaBinary     = (uintptr_t)_binary_job_spurs_job_hello_bin_start;
	job->header.sizeBinary   = CELL_SPURS_GET_SIZE_BINARY(_binary_job_spurs_job_hello_bin_size);

	//Pass the debug flag as the first argument to the spurs job...
	uint8_t *p = (uint8_t*)&job->workArea.userData[0];
	*p = DEBUG_JOB_HELLO;
}

static void 
jobSendEventInit(CellSpursJob128 *job, uint8_t port)
{
	__builtin_memset(job, 0, sizeof(CellSpursJob128));

	job->header.eaBinary     = (uintptr_t)_binary_job_spurs_job_send_event_bin_start;
	job->header.sizeBinary   = (uint16_t)(ceil16((uintptr_t)_binary_job_spurs_job_send_event_bin_size) >> 4);
	uint8_t *p = (uint8_t*)&job->workArea.userData[0];
	*p = port;
}

int runSpursSample(CellSpurs *spurs)
{
	int ret;

	// Create event queue for job chain to signal completion.
	sys_event_queue_attribute_t attrq;
	sys_event_queue_attribute_initialize (attrq);

	sys_event_queue_t queue;
	ret = sys_event_queue_create (&queue, &attrq, SYS_EVENT_QUEUE_LOCAL, 1);
	if (ret)
	{
		fprintf(stderr, "sys_event_queue_create failed : %x\n", ret);
		return (ret);
	}

	uint8_t port;
	ret = cellSpursAttachLv2EventQueue (spurs, queue, &port, true);
	if (ret)
	{
		fprintf(stderr, "cellSpursAttachLv2EventQueue failed : %x\n", ret);
		return (ret);
	}

	// Initialise sample jobs (spurs_job_hello and spurs_job_send_event)
	// and create a job chain...

	jobHelloInit(&JobHello);
	jobSendEventInit(&JobSendEvent, port);

	static uint64_t command_list[4];

	command_list[0] = CELL_SPURS_JOB_COMMAND_JOB(&JobHello);
	command_list[1] = CELL_SPURS_JOB_COMMAND_SYNC;
	command_list[2] = CELL_SPURS_JOB_COMMAND_JOB(&JobSendEvent);
	command_list[3] = CELL_SPURS_JOB_COMMAND_END;

	CellSpursJobChain *jobChain = (CellSpursJobChain *) memalign(128, sizeof(CellSpursJobChain));

	/* initialize job chain attribute */
	const uint8_t priority[8] = {1, 1, 1, 1, 1, 1, 1, 1};
	CellSpursJobChainAttribute attr;
	ret = cellSpursJobChainAttributeInitialize(
		&attr,
		command_list,
		sizeof(JobType), // default job descriptor size
		16,		// max grabbed job
		priority, 
		1,		// max contention
		true,	// auto ready count
		0, 1,	// DMA tags for job streaming
		false,	// fixed memory allocation
		MAX_JOB_DESCRIPTOR_SIZE,
		0		// initial ready count (ignored if auto ready count is true)
		);
	if (ret) {
		printf("cellSpursJobChainAttributeInitialize failed : %x\n", ret);
		printf("## libspurs : " SAMPLE_NAME " FAILED ##\n");
		abort();
	}

	ret = cellSpursJobChainAttributeSetName(&attr, SAMPLE_NAME);
	if (ret) {
		fprintf(stderr, "cellSpursJobChainAttributeSetName failed : %x\n", ret);
		return (ret);
	}

	/* create job chain */
	ret = cellSpursCreateJobChainWithAttribute(spurs, jobChain, &attr);
	if (ret) {
		fprintf(stderr, "cellSpursCreateJobChainWithAttribute failed : %x\n", ret);
		return (ret);
	}

	/* let the job chain start */	
	ret = cellSpursRunJobChain(jobChain);
	if (ret) {
		fprintf(stderr, "cellSpursRunJobChain failed : %x\n", ret);
		return (ret);
	}

	// Wait for job chain to complete...
	sys_event_t	event;
	ret = sys_event_queue_receive(queue, &event, 0);
	if(ret)
	{
		fprintf(stderr, "sys_event_queue_receive failed : %x\n", ret);
		return ret;
	}

	// Cleanup...
	ret = cellSpursShutdownJobChain(jobChain);
	if(ret != CELL_OK)
	{
		fprintf(stderr, "cellSpursShutdownJobChain failed : %x\n", ret);
		return (ret);
	}

	ret = cellSpursJoinJobChain(jobChain);
	if(ret != CELL_OK)
	{
		fprintf(stderr, "cellSpursJoinJobChain failed : %x\n", ret);
		return (ret);
	}

	ret = cellSpursDetachLv2EventQueue (spurs, port);
	if(ret)
	{
		fprintf(stderr, "cellSpursDetachLv2EventQueue failed : %x\n", ret);
		return (ret);
	}

	ret = sys_event_queue_destroy (queue, 0);
	if (ret) 
	{
		fprintf(stderr, "sys_event_queue_destroy failed : %x\n", ret);
		return (ret);
	}

	free(jobChain);

	return CELL_OK;
}
