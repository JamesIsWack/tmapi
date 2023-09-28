/*  SCE CONFIDENTIAL                                                         
*  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
*  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
*  All Rights Reserved.                                                     
*
*  Description: 
*  SN Systems modified version of the policy_module/simple_pm sample
*  included in the PS3 SDK.
*               
*/ 

/* Standard C Libraries */
#include <stdint.h>
#include <stdio.h>						
#include <stdlib.h>						
#include <assert.h>

#include <cell/spurs.h>
#include "spurs_policy_module/policy_module.h"

#ifdef _DEBUG
uint8_t DEBUG_POLICY_MODULE = 1;
#else
uint8_t DEBUG_POLICY_MODULE = 0;
#endif //#ifdef _DEBUG

#define SPURS_SPU_NUM	6

// Policy Module
extern char _binary_pm_spurs_policy_module_bin_start[];
extern char _binary_pm_spurs_policy_module_bin_size[];

// User SPU Program
extern char _binary_spu_spurs_pm_workload_bin_start[];
extern char _binary_spu_spurs_pm_workload_bin_size[];

int runSpursSample(CellSpurs *spurs)
{
	int ret;


	// Create an event queue to wait for completion of the policy module...
	sys_event_queue_attribute_t attr;
	sys_event_queue_attribute_initialize (attr);

	sys_event_queue_t queue;
	ret = sys_event_queue_create (&queue, &attr, SYS_EVENT_QUEUE_LOCAL, 1);
	if (ret)
	{
		fprintf(stderr, "runSpursSample: sys_event_queue_create failed : %x\n", ret);
		return (ret);
	}

	uint8_t port;
	ret = cellSpursAttachLv2EventQueue (spurs, queue, &port, true);
	if (ret)
	{
		fprintf(stderr, "runSpursSample: cellSpursAttachLv2EventQueue failed : %x\n", ret);
		return (ret);
	}


	// Setup the workload for policy module...
	printf("PPU: Setting up policy module workload...\n");

	int num_programs = 128;
	SimplePolicyModuleProgram *spu_programs = (SimplePolicyModuleProgram*)memalign(16, sizeof(SimplePolicyModuleProgram)*num_programs);

	printf("PPU: SPU Program: ea=%x, size=%x\n",(uintptr_t)_binary_spu_spurs_pm_workload_bin_start, (uintptr_t)_binary_spu_spurs_pm_workload_bin_size);

	for (int i = 0; i < num_programs; i++) 
	{
		spu_programs[i].ea_bin = (uint64_t)(uintptr_t)_binary_spu_spurs_pm_workload_bin_start;
		spu_programs[i].bin_size = (uint32_t)(uintptr_t)_binary_spu_spurs_pm_workload_bin_size;
		spu_programs[i].arg = i;
	}

	for (int i = 0; i < 4; i++) 
	{
		uint64_t *u64 = (uint64_t*)_binary_spu_spurs_pm_workload_bin_start;
		printf("[%d] %llx\n", i, u64[i]);
	}

	// Setup policy module structure...
	SimplePolicyModule *simple_pm = (SimplePolicyModule *)memalign(128, sizeof(SimplePolicyModule));

	simple_pm->ea_spurs = (uint64_t)(uintptr_t)spurs;
	simple_pm->ea_programs = (uint64_t)(uintptr_t)spu_programs;
	simple_pm->num_programs = num_programs;
	simple_pm->queue = queue;
	simple_pm->port = port;
	simple_pm->debug = DEBUG_POLICY_MODULE;

	static uint8_t priority[8] = {15, 15, 15, 15, 15, 15, 15, 15};


	// Add the policy module to spurs, passing the SimplePM structure as an argument...
	ret = cellSpursAddWorkload (
		spurs, 
		&simple_pm->id,
		_binary_pm_spurs_policy_module_bin_start,
		(uint32_t)(uintptr_t)(_binary_pm_spurs_policy_module_bin_size + 15)&(~15),
		(uintptr_t)simple_pm, 
		priority, 
		1, 
		SPURS_SPU_NUM);

	if (ret)
	{
		fprintf(stderr, "runSpursSample: cellSpursAddWorkload failed : %x\n", ret);
		return ret;
	}

	cellSpursReadyCountStore(spurs, simple_pm->id, SPURS_SPU_NUM);
	cellSpursWakeUp(spurs);


	//Wait for the policy module to finish running it's workload...
	printf("PPU: Waiting for policy module to complete...\n");

	sys_event_t event;
	sys_event_queue_receive(simple_pm->queue, &event, 0);

	printf("PPU: Policy module work complete\n");


	//Cleanup...
	ret = cellSpursShutdownWorkload((CellSpurs*)(uintptr_t)simple_pm->ea_spurs, simple_pm->id);
	if (ret)
	{
		fprintf(stderr, "runSpursSample: cellSpursShutdownWorkload failed : %x\n", ret);
		return ret;
	}
	ret = cellSpursWaitForWorkloadShutdown((CellSpurs*)(uintptr_t)simple_pm->ea_spurs, simple_pm->id);
	if (ret)
	{
		fprintf(stderr, "runSpursSample: cellSpursWaitForWorkloadShutdown failed : %x\n", ret);
		return ret;
	}
	ret = cellSpursRemoveWorkload((CellSpurs*)(uintptr_t)simple_pm->ea_spurs, simple_pm->id);
	if (ret)
	{
		fprintf(stderr, "runSpursSample: cellSpursRemoveWorkload failed : %x\n", ret);
		return ret;
	}

	free(spu_programs);
	free(simple_pm);

	return 0;
}
