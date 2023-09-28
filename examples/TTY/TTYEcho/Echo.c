///////////////////////////////////////////////////////////////////////////
//																		 //
// Copyright (C) 2008  Sony Computer Entertainment Inc. / SN Systems Ltd //
//																		 //
//		This example illustrates the TTY input feature of PS3Debugger	 //
//							and PS3TM									 //
//																		 //
//		In PS3TM, use the PPU Channel which has the PPU stream mapped.	 //
//	    In PS3Debugger, you'll need to create a similar channel.		 //
//		Start typing in the single channel view	and hit return.			 //
//		The target side code will print what you have typed.			 //	
//		In the channel properties you can request that your input		 //
//		is immediately echo'd to the screen								 //
//																		 //
///////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char ch = '\0';

	puts("EchoTTY: Hit Q to quit!\n");

	do
	{
		int n = fread((void*) &ch, 1, sizeof(ch), stdin);
		if (n && ch != EOF)
			putchar(ch);

	} while( ch != 'Q' );

	puts("EchoTTY: Quitting!\n");

	return EXIT_SUCCESS;
}
