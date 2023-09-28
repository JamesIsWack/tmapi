/****************************************************************************/
//
// Copyright SN Systems Ltd 2011
//
// Description: BasicExamplesb).c is a set of simple examples designed to 
// illustrate some of the PS3Debugger scripting functionality. The script  
// registers menu items in the CallStack, Watch, Locals and Autos Panes
// making frequent use of GetObjectData to determine processID, threadID or address
// parameters.
//
//****************************************************************************/

sn_int32 ThreadGetCallStack(sn_uint32 ProcessID, sn_uint64 ThreadID)
{
	int i;
	SNPS3_CALLSTACK_INFO* pCallStack;
	sn_uint32 NumLevels = 10;
	pCallStack = (SNPS3_CALLSTACK_INFO *)malloc(NumLevels*sizeof(SNPS3_CALLSTACK_INFO));
	if (SN_FAILED(PS3ThreadGetCallStack(ProcessID, ThreadID, pCallStack, &NumLevels)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pCallStack);
		return 0;
	}
	for(i=0;i<NumLevels;i++)
	{
		printf("\nStack Level %d", i);
		printf("\n\tAddress 0x%X",  pCallStack[i].uAddress.word[0]);
		printf("\n\tProto Type %s", pCallStack[i].pszProtoType);
		printf("\n\tReturn Type %s", pCallStack[i].pszReturnType);
	}
	free(pCallStack);
	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetModuleList(sn_uint32 uProcessID)
{
	int i;
	sn_uint32 NumModules;
	sn_uint32* pModuleIDs;

	NumModules = 30;
	pModuleIDs = (sn_uint32 *)malloc(NumModules*sizeof(sn_uint32));
	if (SN_FAILED(PS3GetModuleList(uProcessID, &NumModules, pModuleIDs)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pModuleIDs);
		return 0;
	}

	for(i=0;i<NumModules;i++)
	{
		printf("\nModuleID:0x%04X", pModuleIDs[i]);
	}

	free(pModuleIDs);
	return 1;
}
////////////////////////////////////////////////////
sn_uint32 GetFirstModuleID(sn_uint32 uProcessID)
{
	sn_uint32 NumModules;
	sn_uint32* pModuleIDs;
    sn_uint32 return_id;

    return_id = 0;
	NumModules = 30;
	pModuleIDs = (sn_uint32 *)malloc(NumModules*sizeof(sn_uint32));
	if (SN_FAILED(PS3GetModuleList(uProcessID, &NumModules, pModuleIDs)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pModuleIDs);
		return 0;
	}

    if (NumModules != 0)
        return_id = pModuleIDs[0];

	free(pModuleIDs);
	return return_id;
}
////////////////////////////////////////////////////
sn_int32 GetModuleInfo(sn_uint32 uProcessID)
{
	int i;
	SNPS3_MODULE_INFO* pModuleInfo = NULL;
	sn_uint32 uBuffSize;
    sn_uint32 uModuleID;
    
    
    uModuleID = GetFirstModuleID(uProcessID);
    if (uModuleID == 0)
    {
        printf("Error: No Modules Found");
        return 0;
    }

	if (SN_FAILED(PS3GetModuleInfo(uProcessID, uModuleID, pModuleInfo, &uBuffSize, NULL)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pModuleInfo = (SNPS3_MODULE_INFO*)malloc(sizeof(SNPS3_MODULE_INFO));
	pModuleInfo->Segments = (SNPS3PRXSEGMENT*)malloc(uBuffSize);

	if (SN_FAILED(PS3GetModuleInfo(uProcessID, uModuleID, pModuleInfo, &uBuffSize, pModuleInfo->Segments)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pModuleInfo->Segments);
		free(pModuleInfo);
		return 0;
	}
	printf("\nModule Info:");
	printf("\n\tName: %s", pModuleInfo->Hdr.aName);
	printf("\n\tVersion: %d.%d", pModuleInfo->Hdr.aVersion[0], pModuleInfo->Hdr.aVersion[1]);
	printf("\n\tAttribute: %x", pModuleInfo->Hdr.uAttribute);
	printf("\n\tStart Entry: %x", pModuleInfo->Hdr.uStartEntry);
	printf("\n\tStop Entry: %x", pModuleInfo->Hdr.uStopEntry);
	printf("\n\tElf Name: %s", pModuleInfo->Hdr.aElfName);
	printf("\n\tNumber of Segments: %x", pModuleInfo->Hdr.uNumSegments);

	for(i=0;i<pModuleInfo->Hdr.uNumSegments;i++)
	{
		printf("\n\tSegment %x", i);
		printf("\n\t\tBase 0x%X", pModuleInfo->Segments[i].uBase.word[0]);
		printf("\n\t\tFileSize 0x%X", pModuleInfo->Segments[i].uFileSize.word[0]);
		printf("\n\t\tMem Size 0x%X", pModuleInfo->Segments[i].uMemSize.word[0]);
		printf("\n\t\tIndex 0x%X", pModuleInfo->Segments[i].uIndex.word[0]);
		printf("\n\t\tElf Type 0x%X", pModuleInfo->Segments[i].uElfType.word[0]);
	}

	free(pModuleInfo->Segments);
	free(pModuleInfo);
	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetThreadList(sn_uint32 uProcessID)
{
	sn_uint32 NumPPUThreads, i;
	sn_uint64* pPPUThreadID = NULL;
	sn_uint32 NumSPUThreads;
	sn_uint64* pSPUThreadID = NULL;

	if (SN_FAILED(PS3GetThreadList(uProcessID, &NumPPUThreads, pPPUThreadID, &NumSPUThreads, pSPUThreadID)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pPPUThreadID = (sn_uint64*)malloc(sizeof(sn_uint64) * NumPPUThreads);
	pSPUThreadID = (sn_uint64*)malloc(sizeof(sn_uint64) * NumSPUThreads);

	if (SN_FAILED(PS3GetThreadList(uProcessID, &NumPPUThreads, pPPUThreadID, &NumSPUThreads, pSPUThreadID)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pPPUThreadID);
		free(pSPUThreadID);
		return 0;
	}

	printf("\nWe found %d PPU threads", NumPPUThreads);
	printf("\nWe found %d SPU threads", NumSPUThreads);

	for(i=0;i<NumPPUThreads;i++)
	{
		printf("\n\tPPU ID 0x%X",  pPPUThreadID[i].word[0]);
	}
	for(i=0;i<NumSPUThreads;i++)
	{
		printf("\n\tSPU ID 0x%X", pSPUThreadID[i].word[0]);
	}

	free(pPPUThreadID);
	free(pSPUThreadID);
	return 1;
}
////////////////////////////////////////////////////
sn_int32 ProcessLoad()
{
	const char* pszFileName = "..\\spurs\\task_hello\\Debug\\task_hello.ppu.self";
	const char* pszCmdLine = "";
	sn_uint32 Priority = 0x1000;
	sn_uint32 uProcessID;
	sn_uint64 uThreadID;

	if (SN_FAILED(PS3ProcessLoad(pszFileName, pszCmdLine, Priority, &uProcessID, &uThreadID)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	printf("\nLoaded %s on Process 0x%x and Thread 0x%X", pszFileName, uProcessID, uThreadID.word[0]);

	return 1;
}
////////////////////////////////////////////////////
sn_int32 ProcessList()
{
	sn_uint32 uCount, i;
	sn_uint32* pProcIDBuff = NULL;

	if (SN_FAILED(PS3ProcessList(&uCount, pProcIDBuff)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pProcIDBuff = (sn_uint32*)malloc(sizeof(sn_uint32) * uCount);

	if (SN_FAILED(PS3ProcessList(&uCount, pProcIDBuff)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pProcIDBuff);
		return 0;
	}

	printf("\nThere is %d Processes", uCount);

	for(i=0;i<uCount;i++)
	{
		printf("\n\tProcess %d ID 0x%x", i, pProcIDBuff[i]);
	}

	free(pProcIDBuff);
	return 1;
}
////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	sn_uint32 uProcessID;
	sn_uint64 uThreadID;

	sn_uint32 uWatchItemId[10];
	sn_uint32 uLocalsItemId[10];
	sn_uint32 uAutosItemId[10];
	sn_uint32 uCallStackItemId[10];

	sn_notify Notify;
	BOOL bIsRunning;
	sn_uint32 uSize32 = 4;
	sn_uint32 uSize64 = 8;

	sn_uint32 uStepFlag = 0;

	TextClear();
	printf("This example script adds custom menu items into callstack, watch, locals and autos view context menus which will print information into this view\n");

	AddMenuItem("Is This Thread Running?", &uCallStackItemId[0], "CallstackView");
	AddMenuItem("Thread Start", &uCallStackItemId[1], "CallstackView");
	AddMenuItem("Thread Stop", &uCallStackItemId[2], "CallstackView");
	AddMenuItem("Kill this Process", &uCallStackItemId[3], "CallstackView");
	AddMenuItem("Process Proceed", &uCallStackItemId[4], "CallstackView");

	AddMenuItem("Thread Step", &uWatchItemId[0], "WatchView");
	AddMenuItem("Thread Step Over", &uWatchItemId[1], "WatchView");
	AddMenuItem("Thread Step Out", &uWatchItemId[2], "WatchView");
	AddMenuItem("Change Step Mode", &uWatchItemId[3], "WatchView");

	AddMenuItem("Tell Me the Call Stack", &uLocalsItemId[0], "LocalsView");
	AddMenuItem("Tell Me the Module List", &uLocalsItemId[1], "LocalsView");
	AddMenuItem("Get me some Modules", &uLocalsItemId[2], "LocalsView");

	AddMenuItem("Give me a List of Threads?", &uAutosItemId[0], "AutosView");
	AddMenuItem("Stop it all!", &uAutosItemId[1], "AutosView");
	AddMenuItem("Load a Process", &uAutosItemId[2], "AutosView");
	AddMenuItem("List the Processes", &uAutosItemId[3], "AutosView");

	RequestNotification(NT_CUSTMENU);
	RequestNotification(NT_KEYDOWN);        //  Allows exit from script
	

	while (1)
	{

		GetNotification(NT_ANY, &Notify);
		PS3UpdateTargetInfo();
		if ((sn_uint32)Notify.pParam1 == uCallStackItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nIs This Thread Running?");
			if (SN_FAILED(PS3ThreadIsRunning(uProcessID, uThreadID, &bIsRunning)))
			{
				printf("Error: %s", GetLastErrorString());
			}
			else printf(bIsRunning?"Yes":"No");
		}
		if ((sn_uint32)Notify.pParam1 == uCallStackItemId[1])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nStart Thread");
			PS3ThreadStart(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uCallStackItemId[2])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nStop Thread");
			PS3ThreadStop(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uCallStackItemId[3])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nKill Process 0x%X", uProcessID);
			if (SN_FAILED( PS3ProcessKill(uProcessID)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uCallStackItemId[4])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nContinue Process 0x%X", uProcessID);
			if (SN_FAILED( PS3ProcessStart(uProcessID)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uWatchItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nStep Thread Flag: %x", uStepFlag);
			PS3ThreadStep(uProcessID, uThreadID, uStepFlag);
		}
		if ((sn_uint32)Notify.pParam1 == uWatchItemId[1])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nStep Thread Over Flag: %x", uStepFlag);
			PS3ThreadStepOver(uProcessID, uThreadID, uStepFlag);
		}
		if ((sn_uint32)Notify.pParam1 == uWatchItemId[2])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nStep Thread Out Flag: %x", uStepFlag);
			PS3ThreadStepOut(uProcessID, uThreadID, uStepFlag);
		}
		if ((sn_uint32)Notify.pParam1 == uWatchItemId[3])
		{
			uStepFlag++;
			uStepFlag = (uStepFlag % 3);
			switch(uStepFlag)
			{
			case SM_DONTCARE:
				printf("\nStep Mode Don't Care");
				break;
			case SM_DISASSEMBLY:
				printf("\nStep Mode Disassembly");
				break;
			case SM_SOURCE:
				printf("\nStep Mode Source");
				break;
			default:
				printf("\nUnknown Step Mode");
				break;
			}
		}

		if ((sn_uint32)Notify.pParam1 == uLocalsItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet CS for: 0x%X, 0x%X", uProcessID, uThreadID.word[0]);
			ThreadGetCallStack(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uLocalsItemId[1])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet ML for: 0x%X", uProcessID);
			GetModuleList(uProcessID);
		}
		if ((sn_uint32)Notify.pParam1 == uLocalsItemId[2])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetModuleInfo(uProcessID);
		}
		if ((sn_uint32)Notify.pParam1 == uAutosItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet TL for: 0x%X", uProcessID);
			GetThreadList(uProcessID);
		}
		if ((sn_uint32)Notify.pParam1 == uAutosItemId[1])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nStop Process 0x%X", uProcessID);
			if (SN_FAILED( PS3ProcessStop(uProcessID)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uAutosItemId[2])
		{
			printf("\nLoad Something", uProcessID);
			ProcessLoad();
		}
		if ((sn_uint32)Notify.pParam1 == uAutosItemId[3])
		{
			printf("\nGet PL");
			ProcessList();
		}
	}
	return 0;
}
////////////////////////////////////////////////////

