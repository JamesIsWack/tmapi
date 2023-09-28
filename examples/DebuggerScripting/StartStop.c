//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: StartStop.c demonstrates the safe use of the UnitStop function.
//   While UnitStart will cause the target to immediately start running from
//   its current position, UnitStop will only post the message to stop and then
//   return.  At times the script coder will want to know that the target has
//   definitely stopped.  As such, the following style of code would be required.
//
//   For an example of this code in a broken state (i.e. without the check),
//   see the example StartStopBroken.c
//
//****************************************************************************

int main(int argc, char** argv)
{
	sn_notify Notify;

	RequestNotification(NT_THREAD_STOP);
	RequestNotification(NT_TIMER);
	SetTimerInterval(5000);				//  Shouldn't time out, but best to be sure
	
	PS3StartAllProcesses();		//  UnitStart commands will start the target immediately

	PS3StopAllProcesses();

	GetNotification(NT_ANY, &Notify);

	if (Notify.eNotifyType == NT_TIMER)
	{
		printf ("Timed out\n");
	}
	else
	{
		printf ("Stopped\n");
	}

	PS3StartAllProcesses();

	return 0;
}