/* SCE CONFIDENTIAL
PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004
* Copyright (C) 2006 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#include <stdint.h>
#include <spu_intrinsics.h>
#include <cell/spurs.h>
#include <spu_printf.h>
#include <LibSN_SPU.h>

int  cellSpursTaskMain(qword argTask, uint64_t argTaskset)
{
	(void)argTaskset;

	//Debug flag gets passed through as a parameter to the task...
	uint64_t debug = spu_extract((vec_ullong2)argTask, 0);
	if (debug)
	{
		snPause();
	}
		
	spu_printf ("SPU: Hello world!\n");

	cellSpursShutdownTaskset (cellSpursGetTasksetAddress ());
	cellSpursExit ();
	return 1;
}
