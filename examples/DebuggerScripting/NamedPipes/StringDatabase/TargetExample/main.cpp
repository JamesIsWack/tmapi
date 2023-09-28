/****************************************************************************/
// Copyright SN Systems Ltd 2007
//
// Basic PPU example code to demonstrate use of the scripting named pipe support via
// autoexec.eic type handler callback.
//****************************************************************************/
#include <stdlib.h>

typedef int StringID;
int main(void)
{

	double range_max = 5.0;
	double range_min = 0.0;
	StringID myStringID = 0;

	while(1)
	{
		myStringID = (StringID)((double)rand() / (RAND_MAX + 1) * (range_max - range_min)+ range_min);
	}

	return 0;
}