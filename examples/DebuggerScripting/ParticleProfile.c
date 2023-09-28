/****************************************************************************/
//
// Copyright SN Systems Ltd 2011
//
// Description: ParticleProfile.c is an example of a simple method of profiling
// a program.  This example has been written to work with the
// PS3 Tutorial Sample "01_ppu_particles" and measures the amount of time between
// frames.  After 60 frames, additional information is given such as average
// time between frames and the FPS.
// 
// As this example requires certain variables to be evaluated during execution 
// a debug build should be used.
//
// Pressing 'X' at any point will exit the profiler, remove the break point and
// resume execution (if it had stopped on the break point and not been resumed).
//
// Note that the pausing of execution is required so that the timer is not restarted
// before it can be measured.
//****************************************************************************/

int Profile(sn_uint32 uProcessID, sn_uint64 uThreadID)
{
	sn_notify Notify;
	sn_val Val;
	sn_val Val2;
	sn_uint32 i;
	sn_address stopAddress, stopAddress2;
	float piTimeSamples[60];		//  Stores the last 60 timeDiffs
	sn_uint32 uSampleCount = 0;
	float uAverageTime = 0.0f;
	BOOL bSamplesAcquired = FALSE;	//  Becomes true once 60 samples are reached
	BOOL bContinue = TRUE;
	BOOL bIsRunning = FALSE;

	if (SN_FAILED(PS3EvaluateExpression(uProcessID, uThreadID, &Val, "initParticles")))
	{
		printf("Unable to find function \"initParticles\" : (%d)%s\n", GetLastError(), GetLastErrorString());
		return 0;
	}
	if (Val.type != snval_uint32)
	{
		printf("Unable to evaluate \"initParticles\" : not snval_uint32\n");
		return 0;
	}
	if (SN_FAILED(PS3AddBreakPoint(uProcessID, uThreadID, Val.val.Address, 1)))
	{
		printf("Unable to add break point to \"initParticles\". %s\n", GetLastErrorString());
		//  Do not exit...  break point might already exist meaning this will be fine
	}

	///////////////////////////////////////////////////

	if (SN_FAILED(PS3EvaluateExpression(uProcessID, uThreadID, &Val2, "updateParticles")))
	{
		printf("Unable to find function \"updateParticles\" : (%d)%s\n", GetLastError(), GetLastErrorString());
		return 0;
	}
	if (Val2.type != snval_uint32)
	{
		printf("Unable to evaluate \"updateParticles\" : not snval_uint32\n");
		return 0;
	}
	if (SN_FAILED(PS3AddBreakPoint(uProcessID, uThreadID, Val2.val.Address, 2)))
	{
		printf("Unable to add break point to \"updateParticles\". %s\n", GetLastErrorString());
		//  Do not exit...  break point might already exist meaning this will be fine
	}


	stopAddress= Val.val.Address;
	stopAddress2 = Val2.val.Address;

	RequestNotification(NT_BREAK);
	RequestNotification(NT_KEYDOWN);		//  Allows exit from script
	TextClear();
	uAverageTime=0.0f;
	while (bContinue)
	{
		GetNotification(NT_ANY, &Notify);
	  
		if (Notify.eNotifyType == NT_BREAK)
		{
			if ((sn_uint32)(Notify.pParam1) == 2)
			{
				if (SN_FAILED(PS3EvaluateExpression(uProcessID, uThreadID, &Val, "{stack:main}fDeltaTime")))
				{
					printf("Failed to retrieve fDeltaTime\n");
					Val.val.f32=0.0f;
				}
				if (Val.type == snval_f32)
				{
					piTimeSamples[uSampleCount] = Val.val.f32;

					if (uSampleCount != 0)
					{
						uAverageTime +=piTimeSamples[uSampleCount];
						printf("Time passed (seconds) = %f \n", Val.val.f32);
					}
					uSampleCount++;
				}
				else
				{
					printf("Error: 64bit Integer Expected\n");
				}
				PS3ThreadStart(uProcessID, uThreadID);			
			}

			if ((sn_uint32)(Notify.pParam1) == 1)
			{
				if (uSampleCount > 1)
					uAverageTime /= (uSampleCount-1);
				uSampleCount = 0;
				if (SN_FAILED(PS3EvaluateExpression(uProcessID, uThreadID, &Val, "{stack:main}fTime")))
				{
					printf("Failed to retrieve fTime\n");
					Val.val.f32=0.0f;
				}

				printf("Time passed (seconds) = %f (Avg Frame = %f, FPS = %f)\n", Val.val.f32, uAverageTime, 1f/uAverageTime);
				uAverageTime=0.0f;

				PS3ThreadStart(uProcessID, uThreadID);	
			}
		}
		else if (Notify.eNotifyType == NT_KEYDOWN)
		{
			// if X is pressed, exit profiler
			if ((sn_int32)Notify.pParam1 == 'X')
			{
				printf("Exiting profiler\n");
				bContinue = FALSE;
			}
		}		
	}

	if (SN_FAILED(PS3RemoveBreakPoint(uProcessID, uThreadID, stopAddress)))
	{
		printf ("Failed to remove break point. %s\n", GetLastErrorString());
	}
	if (SN_FAILED(PS3RemoveBreakPoint(uProcessID, uThreadID, stopAddress2)))
	{
		printf ("Failed to remove break point. %s\n", GetLastErrorString());
	}

	PS3ThreadIsRunning(uProcessID, uThreadID, &bIsRunning);

	if (!bIsRunning)
		PS3ThreadStart(uProcessID, uThreadID);

	return 0;
}


int main(int argc, char ** argv)
{
	sn_uint32 uProcessID;
	sn_uint64 uThreadID;
	sn_uint32 uSize32 = 4;	
	sn_uint32 uSize64 = 8;
	sn_notify Notify;
	sn_uint32 uItemId[10];
	AddMenuItem("Profile", &uItemId[0], "DBGSourceView");

	RequestNotification(NT_CUSTMENU);

	while (1)
	{
		GetNotification(NT_CUSTMENU, &Notify);
		if ((sn_uint32)Notify.pParam1 == uItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			PS3RemoveAllBreakPoints(uProcessID, uThreadID);
			Profile(uProcessID, uThreadID);
		}
	}

	return 0;
}

