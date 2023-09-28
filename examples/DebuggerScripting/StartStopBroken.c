//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: StartStopBroken.c demonstrates the incorrect use of the 
//   UnitStop function.  In this example no check is being made to ensure the
//   Unit has stopped.  As such, the Start command is issued before it actually 
//   stops and the target ends up stopped when it should be running.
//
//****************************************************************************


int main(int argc, char** argv)
{
	printf("Starting\n");
	PS3StartAllProcesses();	

	printf("Stopping\n");
	PS3StopAllProcesses();

	printf("Starting\n");
	PS3StartAllProcesses();

	printf("I should be running now...\n");

	return 0;
}