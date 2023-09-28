/*  SCE CONFIDENTIAL                                                         
*  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
*  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
*  All Rights Reserved.                                                     
*/ 

#include <cell/spurs/job_chain.h>
#include <cell/spurs/job_context.h>
#include <spu_printf.h>

void cellSpursJobMain2(CellSpursJobContext2* stInfo, CellSpursJob256 *job256)
{
	(void)stInfo;
	CellSpursJob128 *job = (CellSpursJob128 *)job256;
	uint8_t *port = (uint8_t *)&job->workArea.userData[0];
	spu_printf(" ****** event sender : port=%x\n", *port);
	sys_spu_thread_throw_event(*port, 0, 0);
}
