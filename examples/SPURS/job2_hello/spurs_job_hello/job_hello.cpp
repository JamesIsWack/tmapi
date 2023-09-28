/*  SCE CONFIDENTIAL                                                         
*  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
*  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
*  All Rights Reserved.                                                     
*/ 

#include <cell/spurs/job_chain.h>
#include <cell/spurs/job_context.h>
#include <spu_printf.h>
#include <libsn_spu.h>

void cellSpursJobMain2(CellSpursJobContext2* stInfo, CellSpursJob256 *job256)
{
	(void)stInfo;

	CellSpursJob128 *job = (CellSpursJob128 *)job256;
	uint8_t debug_this_module = *(uint8_t *)&job->workArea.userData[0];
	if (debug_this_module)
	{
		//Adding a pause here will automatically stop the debugger...
		snPause();
	}

	spu_printf("job2 hello\n");
}
