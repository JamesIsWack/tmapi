/*  SCE CONFIDENTIAL                                                         
 *  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
 *  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
 *  All Rights Reserved.                                                     
 *
 *  Description: 
 *  SPURS boiler-plate startup code - makes a call to runSpursSample to 
 *  run the sample code.
 *               
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <sys/spu_initialize.h>
#include <cell/spurs.h>

#include "spu_printf_server.h"

SYS_PROCESS_PARAM(1001, 0x10000)

const uint32_t NUM_SPU = 6;
const uint32_t NUM_SPURS_SPU = 1;
const uint32_t SPURS_THREAD_GROUP_PRIORITY = 250;

extern int runSpursSample(CellSpurs *spurs);

static
int get_thread_priority(int *ppu_pri)
{
	sys_ppu_thread_t ppu_thread_id;
	int ret = sys_ppu_thread_get_id(&ppu_thread_id);
	if(ret)
		return ret; 

	ret = sys_ppu_thread_get_priority(ppu_thread_id, ppu_pri);
	if(ret) 
		return ret; 

	*ppu_pri = *ppu_pri - 1;
	return 0;
}

int main (int argc, char *argv[])
{
	int ret = sys_spu_initialize (NUM_SPU, 0);
	if (ret) 
	{
		fprintf(stderr, "sys_spu_initialize(%d, 0) failed: %d\n", NUM_SPU, ret);
		exit(0);
	}

	CellSpurs *spurs = (CellSpurs*) memalign(CELL_SPURS_ALIGN, sizeof(CellSpurs));

	spuPrintfServer *printfService = new spuPrintfServer;
	ret = printfService->Initialize();
	if (ret)
	{
		fprintf(stderr, "printfService->Initialize() failed: %d\n", ret);
		exit(0);
	}

	int ppu_thr_priority;
	ret = get_thread_priority(&ppu_thr_priority);
	if (ret)
	{
		fprintf(stderr, "get_thread_priority failed: %d\n", ret);
		exit(0);
	}

	ret = cellSpursInitialize (spurs, NUM_SPURS_SPU, SPURS_THREAD_GROUP_PRIORITY, ppu_thr_priority, true);
	if (ret) 
	{
		fprintf(stderr, "cellSpursInitialize failed : %x\n", ret);
		return (ret);
	}

	ret = printfService->RegisterSpursGroup(spurs);
	if (ret)
	{
		fprintf(stderr, "printfService->RegisterSpursGroup failed : %x\n", ret);
		return (ret);
	}

	//Run sample...
	ret = runSpursSample(spurs);
	if (ret)
	{
		fprintf(stderr, "runSpursSample failed : %x\n", ret);
		return (ret);
	}

	ret = printfService->UnRegisterSpursGroup(spurs);
	if (ret)
	{
		fprintf(stderr, "printfService->UnRegisterSpursGroup failed : %x\n", ret);
		return (ret);
	}

	ret = printfService->Destroy();
	if (ret)
	{
		fprintf(stderr, "printfService->Destroy failed : %x\n", ret);
		return (ret);
	}

	ret = cellSpursFinalize (spurs);
	if (ret) 
	{
		fprintf(stderr, "cellSpursFinalize failed : %x\n", ret);
		return ret;
	}

	free (spurs);
	delete printfService;

	printf("Succeeded!\n");
	return 0;
}
