/****************************************************************************/
//
// Copyright SN Systems Ltd 2011
//
// Description: BasicExamples(a).c is a set of simple examples designed to 
// illustrate some of the PS3Debugger scripting functionality. The script  
// registers menu items in the Source and Memory Panes
// making frequent use of GetObjectData to determine processID, threadID or address
// parameters.
//
//****************************************************************************/

char tolower(char ch)
{
	if ( 0x41 <= (sn_uint32)ch && (sn_uint32)ch <=0x60)
		return (char)((sn_uint32)ch + 32);
	return ch;
}
////////////////////////////////////////////////////
void print_expression(sn_val Expr)
{
	switch(Expr.type)
	{
	case snval_int8:
		printf("\n8 bit 0x%x", (sn_uint32) Expr.val.i8);
		break;
	case snval_uint8:
		printf("\n8 bit 0x%x", (sn_uint32) Expr.val.u8);
		break;
	case snval_int16:
		printf("\n16 bit 0x%x", (sn_uint32) Expr.val.i16);
		break;
	case snval_uint16:
		printf("\n16 bit 0x%x", (sn_uint32) Expr.val.u16);
		break;
	case snval_int32:
		printf("\n32 bit 0x%x", Expr.val.i32);
		break;
	case snval_uint32:
		printf("\n32 bit 0x%x", Expr.val.u32);
		break;
	case snval_int64:
		printf("\n64 bit 0x%08x%08x",  Expr.val.i64.word[1], Expr.val.i64.word[0]);
		break;
	case snval_uint64:
		printf("\n64 bit 0x%08x%08x",  Expr.val.u64.word[1], Expr.val.u64.word[1]);
		break;
	case snval_int128:
		printf("\n128 bit 0x%08x%08x%08x%08x", Expr.val.i128.word[3], Expr.val.i128.word[2], Expr.val.i128.word[1], Expr.val.i128.word[0]);
		break;
	case snval_uint128:
		printf("\n128 bit 0x%08x%08x%08x%08x", Expr.val.u128.word[3], Expr.val.u128.word[2], Expr.val.u128.word[1], Expr.val.u128.word[0]);
		break;
	case snval_f32:
		printf("\nFloat %f",Expr.val.f32);
		break;
	case snval_f64:
		printf("\nFloat %f", Expr.val.f64);
		break;
	case snval_address:
		printf("\nAddress is %04x%04x", Expr.val.Address.word[1], Expr.val.Address.word[0]);
		break;
	default:
		printf("\nUnrecognised type\n");
	}
	return;
}
////////////////////////////////////////////////////
sn_int32 GetMemory(sn_uint32 ProcessID, sn_uint64 ThreadID, unsigned char *pBuffer, sn_address address, sn_uint32 uCount)
{
	int i;

	if (SN_FAILED(PS3GetMemory(ProcessID, ThreadID, pBuffer, address, uCount)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

  printf("\n0x");
	for(i=0;i<4;i++) printf("%02X", pBuffer[i]);

	return 1;
}
////////////////////////////////////////////////////
sn_int32 SetMemory(sn_uint32 ProcessID, sn_uint64 ThreadID, sn_uint8 *pBuffer, sn_address address, sn_uint32 uCount)
{
	if (SN_FAILED(PS3SetMemory(ProcessID,ThreadID,pBuffer,address,uCount)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetBreakPoints(sn_uint32 ProcessID, sn_uint64 ThreadID)
{
	int i;
	sn_address address[20];
	sn_uint32 NumBP;

	if (SN_FAILED(PS3GetBreakPoints(ProcessID, ThreadID, address, &NumBP)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	for(i=0;i<NumBP;i++) printf("\nBreakPoint at 0x%08X", address[i].word[0]);

	return 1;
}
////////////////////////////////////////////////////
sn_uint32 GetRawSPUInfo(sn_uint32 uProcessID, sn_uint64 pThreadID)
{
	SNPS3_RAW_SPU_INFO* pRawInfo = NULL;
	sn_uint32 uBuffSize;

	if (SN_FAILED(PS3GetRawSPUInfo(uProcessID,pThreadID, pRawInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pRawInfo = (SNPS3_RAW_SPU_INFO*)malloc(uBuffSize);

	if (SN_FAILED(PS3GetRawSPUInfo(uProcessID,pThreadID, pRawInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pRawInfo);
		return 0;
	}
	printf("\nRaw SPU Info received");
	printf("\n\tID: 0x%x", pRawInfo->uID);
	printf("\n\tParent ID: 0x%x", pRawInfo->uParentID);
	printf("\n\tLast PC Address: 0x%08x%08x", pRawInfo->aLastPC.word[1], pRawInfo->aLastPC.word[0]);

	if (pRawInfo->uNameLen != 0)
		printf("\n\tThread Name: %s", ((char*)pRawInfo + sizeof(SNPS3_RAW_SPU_INFO)));
	if (pRawInfo->uFileNameLen != 0)
		printf("\n\tFile Name: %s", ((char*)pRawInfo + sizeof(SNPS3_RAW_SPU_INFO) + pRawInfo->uNameLen));

	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetPPUThreadInfo(sn_uint32 uProcessID, sn_uint64 pThreadID)
{
	SNPS3_PPU_THREAD_INFO* pThreadInfo = NULL;
	sn_uint32 uBuffSize;

	if (SN_FAILED(PS3GetThreadInfo(uProcessID,pThreadID, pThreadInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pThreadInfo = (SNPS3_PPU_THREAD_INFO*)malloc(uBuffSize);

	if (SN_FAILED(PS3GetThreadInfo(uProcessID,pThreadID, pThreadInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pThreadInfo);
		return 0;
	}
	printf("\nPPU Thread Info received");
	printf("\n\tThread ID: 0x%08x", pThreadInfo->uThreadID.word[0]);
	printf("\n\tPriority 0x%x", pThreadInfo->uPriority);
	printf("\n\tStack Address: 0x%08x", pThreadInfo->uStackAddress.word[0]);
	printf("\n\tStack Size: 0x%08x", pThreadInfo->uStackSize.word[0]);
	printf("\n\tThread Name: %s", ((char*)pThreadInfo + sizeof(SNPS3_PPU_THREAD_INFO)));
	switch(pThreadInfo->uState){
		case(PPU_STATE_IDLE):
			printf("\n\tIdle");
			break;
		case(PPU_STATE_EXECUTABLE):
			printf("\n\tExecutable");
			break;
		case(PPU_STATE_EXECUTING):
			printf("\n\tExecuting");
			break;
		case(PPU_STATE_SLEEPING):
			printf("\n\tSleeping");
			break;
		case(PPU_STATE_SUSPENDED):
			printf("\n\tSuspended");
			break;
		case(PPU_STATE_STOPPED_BUSY):
			printf("\n\tStopped - BUSY");
			break;
		case(PPU_STATE_STOPPED):
			printf("\n\tStopped");
			break;
		default:
			printf("\n\tUnknown");
	}

	free(pThreadInfo);
	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetSPUThreadInfo(sn_uint32 uProcessID, sn_uint64 pThreadID)
{
	SNPS3_SPU_THREAD_INFO* pThreadInfo = NULL;
	sn_uint32 uBuffSize;

	if (SN_FAILED(PS3GetThreadInfo(uProcessID,pThreadID, pThreadInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pThreadInfo = (SNPS3_SPU_THREAD_INFO*)malloc(uBuffSize);

	if (SN_FAILED(PS3GetThreadInfo(uProcessID,pThreadID, pThreadInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pThreadInfo);
		return 0;
	}
	printf("\nSPU Thread Info received");
	printf("\n\tThread ID: 0x%x%08x", pThreadInfo->uThreadID.word[1], pThreadInfo->uThreadID.word[0]);
	printf("\n\tThread Group ID: 0x%x%08x", pThreadInfo->uThreadGroupID.word[1], pThreadInfo->uThreadGroupID.word[0]);
	if (pThreadInfo->uThreadNameLen != 0)
		printf("\n\tThread Name: %s", ((char*)pThreadInfo + sizeof(SNPS3_SPU_THREAD_INFO)));
	if (pThreadInfo->uFileNameLen != 0)
		printf("\n\tFile Name: %s", ((char*)pThreadInfo + sizeof(SNPS3_SPU_THREAD_INFO) + pThreadInfo->uThreadNameLen));

	free(pThreadInfo);
	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetSPUThreadGroupInfo(sn_uint32 uProcessID, sn_uint64 pThreadID)
{
	SNPS3_SPU_THREAD_INFO* pThreadInfo = NULL;
	sn_uint32 uBuffSize;

	SNPS3_SPU_THREADGROUP_INFO* pThreadGroupInfo = NULL;
	sn_uint32 uBuffSizeG;

	if (SN_FAILED(PS3GetThreadInfo(uProcessID,pThreadID, pThreadInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pThreadInfo = (SNPS3_SPU_THREAD_INFO*)malloc(uBuffSize);

	if (SN_FAILED(PS3GetThreadInfo(uProcessID,pThreadID, pThreadInfo, &uBuffSize)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pThreadInfo);
		return 0;
	}

	printf("\n\tThread Group ID: 0x%x%08x", pThreadInfo->uThreadGroupID.word[1], pThreadInfo->uThreadGroupID.word[0]);

	if (SN_FAILED(PS3GetSPUThreadGroupInfo(uProcessID, pThreadInfo->uThreadGroupID, pThreadGroupInfo, &uBuffSizeG)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pThreadInfo);
		return 0;
	}
	pThreadGroupInfo = (SNPS3_SPU_THREADGROUP_INFO*)malloc(uBuffSizeG);

	if (SN_FAILED(PS3GetSPUThreadGroupInfo(uProcessID, pThreadInfo->uThreadGroupID, pThreadGroupInfo, &uBuffSizeG)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pThreadGroupInfo);
		free(pThreadInfo);
		return 0;
	}
	printf("\n Thread Group 0x%x%08x Info:", pThreadGroupInfo->uThreadGroupID.word[1], pThreadGroupInfo->uThreadGroupID.word[0]);
	printf("\n\tPriority:%x ", pThreadGroupInfo->uPriority);
	printf("\n\tNum Threads:%x ", pThreadGroupInfo->uNumThreads);
	printf("\n\tName:%s", ((char*)pThreadGroupInfo + sizeof(SNPS3_SPU_THREADGROUP_INFO)));
	switch(pThreadGroupInfo->uState){
		case(SPU_STATE_NOT_CONFIGURED):
			printf("\n\tNot Configured");
			break;
		case(SPU_STATE_CONFIGURED):
			printf("\n\tConfigured");
			break;
		case(SPU_STATE_READY):
			printf("\n\tReady");
			break;
		case(SPU_STATE_WAITING):
			printf("\n\tWaiting");
			break;
		case(SPU_STATE_SUSPENDED):
			printf("\n\tSuspended");
			break;
		case(SPU_STATE_WAITING_SUSPENDED):
			printf("\n\tWaiting & Suspended");
			break;
		case(SPU_STATE_RUNNING):
			printf("\n\tRunning");
			break;
		case(SPU_STATE_STOPPED):
			printf("\n\tStopped");
			break;
		default:
			printf("\n\tUnknown");
			break;
	}

	free(pThreadInfo);
	free(pThreadGroupInfo);
	return 1;
}
////////////////////////////////////////////////////
sn_int32 GetProcessInfo(sn_uint32 uProcessID)
{
	SNPS3_PROCESS_INFO* pProcessInfo = NULL;
	sn_uint32 uBufferSize;
	int i;

	if (SN_FAILED(PS3GetProcessInfo(uProcessID, pProcessInfo, &uBufferSize, NULL)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pProcessInfo = (SNPS3_PROCESS_INFO*)malloc(sizeof(SNPS3_PROCESS_INFO));
	pProcessInfo->ThreadIDs = (sn_uint64*)malloc(uBufferSize);

	if (SN_FAILED(PS3GetProcessInfo(uProcessID, pProcessInfo, &uBufferSize, pProcessInfo->ThreadIDs)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pProcessInfo->ThreadIDs);
		free(pProcessInfo);
		return 0;
	}

	printf("\nProcess Info:");
	printf("\n\t Num PPU Threads: %x", pProcessInfo->Hdr.uNumPPUThreads);
	printf("\n\t Num SPU Threads: %x", pProcessInfo->Hdr.uNumSPUThreads);
	printf("\n\t Parent Process ID: %x", pProcessInfo->Hdr.uParentProcessID);
	printf("\n\t Max Mem Size: 0x%x%08x", pProcessInfo->Hdr.uMaxMemorySize.word[1], pProcessInfo->Hdr.uMaxMemorySize.word[0]);
	printf("\nPath: %s", pProcessInfo->Hdr.szPath);

	for(i=0;i<(pProcessInfo->Hdr.uNumPPUThreads+pProcessInfo->Hdr.uNumSPUThreads);i++)
	{
		printf("\n\tThreadID: 0x%x%08x", pProcessInfo->ThreadIDs[i].word[1], pProcessInfo->ThreadIDs[i].word[0]);
	}

	switch(pProcessInfo->Hdr.uStatus){
		case(PS_STATE_CREATING):
			printf("\n\tState Creating");
			break;
		case(PS_STATE_READY):
			printf("\n\tState Ready");
			break;
		case(PS_STATE_EXITED):
			printf("\n\tState Exited");
			break;
		case(PS_STATE_UNKNOWN):
			printf("\n\tState Unknown");
			break;
		default:
			printf("\n\tInvalid State");
			break;
	}


	free(pProcessInfo->ThreadIDs);
	free(pProcessInfo);
	return 1;
}
////////////////////////////////////////////////////
sn_uint32 AddressToLine(sn_uint32 uProcessID, sn_uint64 uThreadID, sn_address address)
{
	char *pName = NULL;
	sn_uint32 uLen;
	sn_uint32 uLine;

	if (SN_FAILED(PS3AddressToLine(uProcessID, uThreadID, address, pName, &uLen, &uLine)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	pName = (char*)malloc(uLen * sizeof(char));

	if (SN_FAILED(PS3AddressToLine(uProcessID, uThreadID, address, pName, &uLen, &uLine)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pName);
		return 0;
	}

	printf("\nInfo from Address 0x%X", address.word[0]);
	printf("\n\tLine Number %d", uLine);
	printf("\n\tName %s", pName);

	free(pName);
	return 0;
}
////////////////////////////////////////////////////
sn_uint32 AddressToLabel(sn_uint32 uProcessID, sn_uint64 uThreadID, sn_address address)
{

	char *pName = NULL;
	sn_uint32 uLen;

	if (SN_FAILED(PS3AddressToLabel(uProcessID, uThreadID, address, pName, &uLen)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	pName = (char*)malloc(uLen * sizeof(char));

	if (SN_FAILED(PS3AddressToLabel(uProcessID, uThreadID, address, pName, &uLen)))
	{
		printf("Error: %s", GetLastErrorString());
		free(pName);
		return 0;
	}

	printf("\nInfo from Address 0x%x%08x", address.word[0], address.word[1]);
	printf("\n\tName %s", pName);
	free(pName);
	return 0;
}
////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	sn_uint32 uProcessID;
	sn_uint64 uThreadID;
	unsigned char pBuffOut[4];

	sn_uint32 uSrcItemId[20];
	sn_uint32 uMemItemId[10];
	sn_uint32 uWatchItemId[10];
	sn_uint32 uLocalsItemId[10];
	sn_uint32 uAutosItemId[10];
	sn_uint32 uCallStackItemId[10];

	sn_notify Notify;
	sn_address * pAddress;
	BOOL bIsRunning;
	sn_uint32 uSize32 = 4;
	sn_uint32 uSize64 = 8;
	sn_uint32 i;
	sn_uint32* pBad;
	sn_uint32 ret_val;
	sn_val result;
	unsigned char pBuffIn[4];

	unsigned char pFileName[70];
	unsigned char pLabelName[128];
	sn_uint32 Length;
	sn_uint32 LineNumber;
	sn_uint32 NumUnits;

	char pInputBuffer[128];
	char *pExpressionInput;
	char *pNameInput;
	sn_uint32 uInputLength;

	BOOL bContinueEval;
	char *pName = NULL;
	sn_uint32 uLen;
	sn_uint32 uLine;

	pAddress = malloc(uSize64);
	pBuffIn[0] = 0xDE;
	pBuffIn[1] = 0xAD;
	pBuffIn[2] = 0xBE;
	pBuffIn[3] = 0xEF;

	AddMenuItem("Get Memory At Cursor", &uSrcItemId[0], "DBGSourceView");
	AddMenuItem("Set Memory At Cursor", &uSrcItemId[1], "DBGSourceView");
	AddMenuItem("Save Bin From Here", &uSrcItemId[2], "DBGSourceView");
	AddMenuItem("Load Bin To Here", &uSrcItemId[3], "DBGSourceView");
	AddMenuItem("Run to Address", &uSrcItemId[4], "DBGSourceView");
	AddMenuItem("Get PPU Thread Info", &uSrcItemId[5], "DBGSourceView");
	AddMenuItem("Get SPU Thread Info", &uSrcItemId[6], "DBGSourceView");
	AddMenuItem("Get SPU Thread Group Info", &uSrcItemId[7], "DBGSourceView");
	AddMenuItem("Get Process Info", &uSrcItemId[8], "DBGSourceView");
	AddMenuItem("Evaluate Some Expressions", &uSrcItemId[9], "DBGSourceView");
	AddMenuItem("Where in the Source File?", &uSrcItemId[10], "DBGSourceView");
	AddMenuItem("Is This a Label?", &uSrcItemId[11], "DBGSourceView");
	AddMenuItem("Get Raw SPU Info", &uSrcItemId[12], "DBGSourceView");
	AddMenuItem("Line to Address", &uSrcItemId[13], "DBGSourceView");

	AddMenuItem("Add BreakPoint Here", &uMemItemId[0], "MemoryView");
	AddMenuItem("Remove BreakPoint From Here", &uMemItemId[1], "MemoryView");
	AddMenuItem("Remove All BreakPoints", &uMemItemId[2], "MemoryView");
	AddMenuItem("Print BP List", &uMemItemId[3], "MemoryView");
	AddMenuItem("Flush the Memory Cache", &uMemItemId[4], "MemoryView");

	RequestNotification(NT_CUSTMENU);
	RequestNotification(NT_KEYDOWN);        //  Allows exit from script
	TextClear();

	printf("This example script adds custom menu items into source and memory view context menus which will print information into this view\n");

	while (1)
	{

		GetNotification(NT_ANY, &Notify);
		PS3UpdateTargetInfo();
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nGetting Address 0x%08X\n", pAddress->word[0]);
			GetMemory(uProcessID,uThreadID,pBuffOut,*pAddress, 4);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[1])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nSetting Address 0x%08X\n", pAddress->word[0]);
			SetMemory(uProcessID,uThreadID,pBuffIn,*pAddress, 4);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[2])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nSave Bin From 0x%08X\n", pAddress->word[0]);
			PS3SaveBin(uProcessID, uThreadID, "Temp.bin", *pAddress, 16);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[3])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nLoad Bin to 0x%08X\n", pAddress->word[0]);
			PS3LoadBin(uProcessID, uThreadID, "Temp.bin", *pAddress);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[4])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nRun to 0x%08X\n", pAddress->word[0]);
			if (SN_FAILED(PS3ThreadRunToAddress(uProcessID, uThreadID, *pAddress)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[5])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet Thread Info for: %x, 0x%x", uProcessID, uThreadID.word[0]);
			GetPPUThreadInfo(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[6])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet Thread Info for: %x, 0x%x", uProcessID, uThreadID.word[0]);
			GetSPUThreadInfo(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[7])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet Thread Group Info for: %x, 0x%x", uProcessID, uThreadID.word[0]);
			GetSPUThreadGroupInfo(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[8])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nGet Process Info for: %x", uProcessID);
			GetProcessInfo(uProcessID);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[10])
		{
			printf("\nWhat line of source?");
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			AddressToLine(uProcessID, uThreadID, *pAddress);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[11])
		{
			printf("\nLook for Label\n");
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			AddressToLabel(uProcessID, uThreadID, *pAddress);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[12])
		{
			printf("\nRaw Info\n");
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetRawSPUInfo(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[9])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nEvaluate in the Thread 0x%x and Process %x context\n", uThreadID.word[0], uProcessID);
			bContinueEval = TRUE;
			while(bContinueEval)
			{
				printf(">>");
                GetInputLine(pInputBuffer, sizeof(pInputBuffer)/sizeof(char), &uInputLength);

				if (SN_FAILED(PS3EvaluateExpression(uProcessID,uThreadID, &result, pInputBuffer)))
				{
					printf("\nError: %s", GetLastErrorString());
				}
				else
				{
					print_expression(result);
				}

				while (bContinueEval)
				{
					printf("\nx to exit, c to continue\n");
					CancelNotification(NT_KEYDOWN);
					RequestNotification(NT_KEYDOWN);
					GetNotification(NT_KEYDOWN, &Notify);
					if ((sn_uint32)Notify.pParam1 == 'X')
						bContinueEval = FALSE;
					else if ((sn_uint32)Notify.pParam1 == 'C')
						break;
				}
			}
			printf("Finished evaluating\n\n");
		}
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[13])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nAddress at line 20 for thread 0x%X and Process 0x%X context\n", uThreadID.word[0], uProcessID);
			printf("\nWhich File?\n");
			bContinueEval = TRUE;
			while(bContinueEval)
			{
				printf(">>");
				GetInputLine(pInputBuffer, sizeof(pInputBuffer)/sizeof(char), &uInputLength);
				uLine = 20;
				if (SN_FAILED(PS3LineToAddress(uProcessID,uThreadID, pAddress, pInputBuffer, uLine )))
				{
					printf("\nError: %s", GetLastErrorString());
				}
				else
				{
					printf("\nAddress at Line %d is 0x%08X\n", uLine,  pAddress->word[0]);
				}
				
				while (bContinueEval)
				{
					printf("\nx to exit, c to continue\n");
					CancelNotification(NT_KEYDOWN);
					RequestNotification(NT_KEYDOWN);
					GetNotification(NT_KEYDOWN, &Notify);
					if ((sn_uint32)Notify.pParam1 == 'X')
						bContinueEval = FALSE;
					else if ((sn_uint32)Notify.pParam1 == 'C')
						break;
					else
						printf("Invalid option : %c\n", Notify.pParam1);
				}
			}
		}
		if ((sn_uint32)Notify.pParam1 == uMemItemId[0])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nSet BreakPoint At 0x%08X\n", pAddress->word[0]);
			if (SN_FAILED(PS3AddBreakPoint(uProcessID, uThreadID, *pAddress, 1)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uMemItemId[1])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetObjectData(LOWORD(Notify.pParam2), GVD_ADDR_AT_CURSOR, pAddress, uSize64);
			printf("\nRemove BreakPoint From 0x%08X\n", pAddress->word[0]);
			if (SN_FAILED(PS3RemoveBreakPoint(uProcessID, uThreadID, *pAddress)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uMemItemId[2])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nRemove All BreakPoints");
			if (SN_FAILED(PS3RemoveAllBreakPoints(uProcessID, uThreadID)))
			{
				printf("Error: %s", GetLastErrorString());
			}
		}
		if ((sn_uint32)Notify.pParam1 == uMemItemId[3])
		{
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("\nPrint BP List");
			GetBreakPoints(uProcessID, uThreadID);
		}
		if ((sn_uint32)Notify.pParam1 == uMemItemId[4])
		{
			printf("\nFlush Memory Cache");
			GetObjectData(LOWORD(Notify.pParam2), GVD_THREAD, &uThreadID, uSize64);
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			PS3FlushMemoryCache(uProcessID, uThreadID);
		}
	}
	free(pAddress);
	return 0;
}
////////////////////////////////////////////////////
