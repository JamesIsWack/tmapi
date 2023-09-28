/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007  Sony Computer Entertainment Inc. / SN Systems Ltd
//
/////////////////////////////////////////////////////////////////////////

#include <libsn_spu.h>

unsigned int EntryPoint( void ) __attribute__((section(".text.startup")));

unsigned int EntryPoint(void)
{
	return 43;
}
