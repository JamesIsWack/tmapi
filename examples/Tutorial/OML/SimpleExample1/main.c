#include <stdio.h>
#include <sys/timer.h>

#include "libsn.h"

extern const char *GetText();

int main()
{
	while (1)
	{
		puts(GetText());
		sys_timer_sleep(3);
		//snSafeLoadPoint();
	}
}
