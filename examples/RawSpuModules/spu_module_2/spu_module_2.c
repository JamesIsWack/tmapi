#include <libsn_spu.h>

unsigned int EntryPoint( void ) __attribute__((section(".text.startup")));

unsigned int EntryPoint(void)
{
	return 43;
}
