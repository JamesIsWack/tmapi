/*  SCE CONFIDENTIAL                                                         
 *  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
 *  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
 *  All Rights Reserved.                                                     
 *
 *  Description: 
 *  SN Systems modified version of the task_hello sample included in the SDK.
 *
 *  Demonstrates SPURs debugging using calls to snPause() in the task you wish
 *  to debug
 *               
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <cell/spurs.h>

static const uint32_t NUM_SPURS_SPU = 1;

extern char _binary_task_spurs_task_hello_elf_start[];	

//Use a global var to specify job debugging.
#ifdef _DEBUG
const uint64_t DEBUG_TASK = 1;
#else
const uint64_t DEBUG_TASK = 0;
#endif 

int runSpursSample (CellSpurs *spurs)
{
	int ret;

	// Create taskset...
	CellSpursTaskset *taskset = (CellSpursTaskset *) memalign(CELL_SPURS_TASKSET_ALIGN, sizeof(CellSpursTaskset));

	uint8_t prios[8] = {1, 1, 1, 1, 1, 1, 1, 1};

	ret = cellSpursCreateTaskset (
		spurs, 
		taskset, 
		(uint64_t)0, 
		prios, 
		NUM_SPURS_SPU);

	if (ret) 
	{
		fprintf(stderr, "cellSpursCreateTaskset failed: %d\n", ret);
		return ret;
	}

	CellSpursTaskArgument Args;
	Args.u64[0] = DEBUG_TASK;

	// Create a task...
	CellSpursTaskId	tid;
	ret = cellSpursCreateTask(
		taskset, 
		&tid, 
		_binary_task_spurs_task_hello_elf_start, 
		(void*)0, 
		0, 
		(CellSpursTaskLsPattern*) 0, 
		&Args);

	if (ret) 
	{
		fprintf(stderr, "cellSpursCreateTask failed : %d\n", ret);
		return ret;
	}

	// Wait for the task set to complete...
	ret = cellSpursJoinTaskset(taskset);
	if (ret) 
	{
		fprintf(stderr, "cellSpursJoinTaskset failed : %d\n", ret);
		return ret;
	}

	printf("Sample completed.\n");

	free(taskset);

	return 0;
}
