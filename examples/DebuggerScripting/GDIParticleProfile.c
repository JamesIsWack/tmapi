/****************************************************************************/
//
// Copyright SN Systems Ltd 2011
//
// Description: GDIParticleProfile.c is a simple example of profiling
// a program.  This example has been written to work with the
// PS3 Tutorial Sample "01_ppu_particles" and measures the amount of time between
// frames; displaying the results graphically. 
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



#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 300
#define WINDOW2_HEIGHT 100
#define SCALE_FACTOR 2
#define SCALE2_FACTOR 2

// draws the graphs, axes and titles
void drawBaseGraph()
{
	sn_uint32 i;
	char buffer[3];

	TextClear();
	GraphicsClearWindow();

	GraphicsSetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT + WINDOW2_HEIGHT);
	GraphicsSetDrawMode(DF_TEXT | DF_GRAPHICS,DM_SRC_AND_DST);
	GraphicsSetForegroundColor(255, 255, 255);
	// Top Graph
	GraphicsDrawRectangle(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
	GraphicsSetForegroundColor(0,0,255);
	for(i=1;i<=20;i++)
		GraphicsDrawText((WINDOW_WIDTH*i)/22, 16*WINDOW_HEIGHT/20, _itoa(i, buffer, 10));

	GraphicsDrawLine((WINDOW_WIDTH)/22, 15*WINDOW_HEIGHT/20,(19*WINDOW_WIDTH)/20,15*WINDOW_HEIGHT/20 );
	GraphicsDrawLine((WINDOW_WIDTH)/22, 15*WINDOW_HEIGHT/20,(WINDOW_WIDTH)/22,2*WINDOW_HEIGHT/20 );
	GraphicsDrawText(9*WINDOW_WIDTH/20, 18*WINDOW_HEIGHT/20, "Frame");

	GraphicsDrawText(WINDOW_WIDTH/40, 11*WINDOW_HEIGHT/30, "T");
	GraphicsDrawText(WINDOW_WIDTH/40, 12*WINDOW_HEIGHT/30, "i");
	GraphicsDrawText(WINDOW_WIDTH/40, 13*WINDOW_HEIGHT/30, "m");
	GraphicsDrawText(WINDOW_WIDTH/40, 14*WINDOW_HEIGHT/30, "e");

	GraphicsSetForegroundColor(137,58,148);
	GraphicsDrawText(8*WINDOW_WIDTH/20, WINDOW_HEIGHT/20, "Frame Time");

	// Lower Graph
	GraphicsSetForegroundColor(255, 255, 255);
	GraphicsDrawRectangle(0,WINDOW_HEIGHT,WINDOW_WIDTH,WINDOW_HEIGHT+WINDOW2_HEIGHT);
	GraphicsSetForegroundColor(0,0,255);
	for(i=1;i<=20;i++)
		GraphicsDrawText((WINDOW_WIDTH*i)/22, WINDOW_HEIGHT+16*WINDOW2_HEIGHT/20, _itoa(i, buffer, 10));

	GraphicsDrawLine((WINDOW_WIDTH)/22, WINDOW_HEIGHT+ 15*WINDOW2_HEIGHT/20,(19*WINDOW_WIDTH)/20,WINDOW_HEIGHT+15*WINDOW2_HEIGHT/20 );
	GraphicsDrawLine((WINDOW_WIDTH)/22, WINDOW_HEIGHT+ 15*WINDOW2_HEIGHT/20,(WINDOW_WIDTH)/22,WINDOW_HEIGHT+2*WINDOW2_HEIGHT/20 );

	GraphicsSetForegroundColor(189,17,17);
	GraphicsDrawText(6*WINDOW_WIDTH/20, WINDOW_HEIGHT+WINDOW2_HEIGHT/20, "Average Frame Time for Particle Set");
	GraphicsUpdate();
}

// Main Profile Loop, waiting for stop and key notifications
int Profile(sn_uint32 uProcessID, sn_uint64 uThreadID)
{
	sn_notify Notify;
	sn_val Val, Val2;
	sn_address stopAddress, stopAddress2;
	sn_uint32 i;

	float pfFullTimeAve[20]; //Holds particle set ave times
	float fTotalInitTime = 0; //Holds total time for particle set

	sn_uint32 uParticlesInSet = 0; 
	sn_uint32 uNumParticleSets = 0;
	sn_uint32 uSampleCount = 1;
	float uAverageTime = 0.0f;
	float ffullTime = 0.0f;

	BOOL bSamplesAcquired = FALSE;
	BOOL bContinue = TRUE;
	BOOL bIsRunning = FALSE;
	BOOL bNewSet = FALSE;

	drawBaseGraph();
	///////////////////////////////////////////////////
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
	///////////////////////////////////////////////////

	stopAddress= Val.val.Address;
	stopAddress2 = Val2.val.Address;

	RequestNotification(NT_BREAK);
	RequestNotification(NT_KEYDOWN);		

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
					if (!bNewSet)
					{
						if (uSampleCount == 21)
						{
							drawBaseGraph();
							if (uAverageTime != 0)
							{
								GraphicsSetForegroundColor(255,80,0);
								GraphicsDrawLine((WINDOW_WIDTH)/20, 15*WINDOW_HEIGHT/20 - uAverageTime/(uSampleCount-1) * WINDOW_HEIGHT/SCALE_FACTOR,(19*WINDOW_WIDTH)/20,15*WINDOW_HEIGHT/20 - uAverageTime/(uSampleCount-1) * WINDOW_HEIGHT/SCALE_FACTOR );
								for (i=0; i<uNumParticleSets; i++)
								{
									GraphicsSetForegroundColor(70,0,70);
									GraphicsDrawRectangle((1+i)*(WINDOW_WIDTH)/22, WINDOW_HEIGHT+ 15*WINDOW2_HEIGHT/20, (3+2*i)*(WINDOW_WIDTH)/44,(WINDOW_HEIGHT+ 15*WINDOW2_HEIGHT/20) - pfFullTimeAve[i]* WINDOW2_HEIGHT/SCALE2_FACTOR );
								}
								GraphicsUpdate();
							}
							uSampleCount = 0;
							uAverageTime = 0;
						}
						else
						{
							fTotalInitTime += Val.val.f32;
							uAverageTime += Val.val.f32;
							GraphicsSetForegroundColor(31,175,41); 
							GraphicsDrawRectangle(uSampleCount*WINDOW_WIDTH/22,15*WINDOW_HEIGHT/20,(1+2*uSampleCount)*WINDOW_WIDTH/44, 15*WINDOW_HEIGHT/20 - Val.val.f32 * WINDOW_HEIGHT/SCALE_FACTOR);                                      
							GraphicsUpdate();
						}
						uParticlesInSet++;
						uSampleCount++;

					}
					else
						bNewSet = FALSE;
				}
				else
				{
					printf("Error: 64bit Integer Expected\n");
				}

				PS3ThreadStart(uProcessID, uThreadID);			
			}
			if ((sn_uint32)(Notify.pParam1) == 1)
			{
				GraphicsSetForegroundColor(100,80,10);
				GraphicsDrawLine((4*uSampleCount-1)*WINDOW_WIDTH/88,15*WINDOW_HEIGHT/20,(4*uSampleCount-1)*WINDOW_WIDTH/88, WINDOW_HEIGHT/10);
				bNewSet = TRUE;
				if (uParticlesInSet != 0)
				{
					pfFullTimeAve[uNumParticleSets] = fTotalInitTime/uParticlesInSet;
					GraphicsSetForegroundColor(70,0,70);
					GraphicsDrawRectangle((1+uNumParticleSets)*(WINDOW_WIDTH)/22, WINDOW_HEIGHT+ 15*WINDOW2_HEIGHT/20, (3+2*uNumParticleSets)*(WINDOW_WIDTH)/44,(WINDOW_HEIGHT+ 15*WINDOW2_HEIGHT/20) - pfFullTimeAve[uNumParticleSets]* WINDOW2_HEIGHT/SCALE2_FACTOR );
					uNumParticleSets++;
				}
				if (uNumParticleSets >= 20)
					uNumParticleSets = 0;
				uParticlesInSet = 0;
				fTotalInitTime = 0;
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

	TextClear();
	GraphicsClearWindow();
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

