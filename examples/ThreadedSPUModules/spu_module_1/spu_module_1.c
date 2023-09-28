/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007  Sony Computer Entertainment Inc. / SN Systems Ltd
//
/////////////////////////////////////////////////////////////////////////

#include <libsn_spu.h>

unsigned int EntryPoint( void ) __attribute__((section(".text.startup")));

extern int otherfunc1(int a);
extern int otherfunc2(int a);

extern int myvar;

int myothervar = 0xfaaf;


unsigned int EntryPoint(void)
{
	unsigned int a = otherfunc1(2);
	a = otherfunc2(a);
	return a + myvar + myothervar;
}
