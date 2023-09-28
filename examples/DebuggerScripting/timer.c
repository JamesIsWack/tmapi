//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: Timer.c shows how to use the Notify system to handle custom 
// timer events.
//                                                                          
//****************************************************************************

// This is the function that gets called by the timer events.
// It just increments a counter and displays the result.
//
void TimerFunction()
{
	static sn_uint32 s_uCount = 0;
	printf("Timer counter: %i\n", ++s_uCount);
	return;
}

//
// This part sets up a notfication to receive timer events and then sets up a regular 
// 1 second timer event.
//
int main(int argc, char** argv)
{
	sn_notify Notify;
		
	RequestNotification(NT_TIMER);
	SetTimerInterval(1000);
	printf("This example script sets up timer notification\n");
	while (1)
	{
		GetNotification(NT_TIMER, &Notify);			//  Only care about NT_TIMER notifications
		if (Notify.eNotifyType == NT_TIMER)
            TimerFunction();	
	}
	return 0;
}
