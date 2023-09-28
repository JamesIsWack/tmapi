//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: 
//  Illustrates use of PS3GetEventQueueInfo and PS3GetEventQueueList
// 
//
//****************************************************************************
sn_uint32 GetEventQueueList(sn_uint32 uProcessID)
{

	sn_uint32 uCount;
	sn_uint32* pBuffer = NULL;
	sn_uint32 i, j;
	sn_uint64* pWaitingThreads;
	SNPS3_SYS_EVENT* pEventQueueEntries;
			
	SNPS3_EVENT_QUEUE_INFO* pInfo;
	sn_uint32 IDBuffsize = 10 * sizeof(sn_uint64);
	sn_uint32 EQEntiresBuffSize = 20 * sizeof(SNPS3_SYS_EVENT);

	if (SN_FAILED(PS3GetEventQueueList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	
	pBuffer = (sn_uint32*)malloc(uCount * sizeof(sn_uint32));
		
	if (SN_FAILED(PS3GetEventQueueList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	////////////////////////////////////////////////////////////
	printf("List of %d Event Queue IDs:\n", uCount);
	
	pInfo = (SNPS3_EVENT_QUEUE_INFO*)malloc(sizeof(SNPS3_EVENT_QUEUE_INFO));
	pInfo->pWaitThreadIDs = (sn_uint64*)malloc(10 * sizeof(sn_uint64));
	pInfo->pEventQueueEntries = (SNPS3_SYS_EVENT*)malloc(20 * sizeof(SNPS3_SYS_EVENT));
	
	for (i = 0; i < uCount ; i++)
	{
		if (SN_FAILED(PS3GetEventQueueInfo(uProcessID, pBuffer[i], pInfo, pInfo->pWaitThreadIDs,  &IDBuffsize, pInfo->pEventQueueEntries, &EQEntiresBuffSize)))
			continue;

		printf("\nID 0x%x \n\tprotocol=0x%x Type=0x%x name=%s ID=0x%x Key 0x%x%x QueueSize 0x%x", pBuffer[i], pInfo->Attribute.uAttrProtocol, pInfo->Attribute.uType, pInfo->Attribute.aName, pInfo->uEventQueueID, pInfo->uKey.word[0], pInfo->uKey.word[1], pInfo->uQueueSize);
		pWaitingThreads = pInfo->pWaitThreadIDs;
		pEventQueueEntries = pInfo->pEventQueueEntries;
		
		printf("\n%d threads waiting", pInfo->uWaitThreadsNum);
		for (j = 0; j < pInfo->uWaitThreadsNum; j++)
		{
			printf("\nWaiting thread ID 0x%x%x",  pWaitingThreads->word[1], pWaitingThreads->word[0]);
			pWaitingThreads++;
		}
		for (j = 0; j < pInfo->uReadableEqueueNum; j++)
		{
			printf("\nEvent Queue %d Source 0x%x%x Data 0x%x%x 0x%x%x 0x%x%x",  j, pEventQueueEntries->uSource.word[0], pEventQueueEntries->uSource.word[1], pEventQueueEntries->uData1.word[0], pEventQueueEntries->uData1.word[1], pEventQueueEntries->uData2.word[0], pEventQueueEntries->uData2.word[1], pEventQueueEntries->uData3.word[0], pEventQueueEntries->uData3.word[1]);
			pEventQueueEntries++;
		}
	}
	
	free(pInfo->pEventQueueEntries);
	free(pInfo->pWaitThreadIDs);
	free(pInfo);
	
	return 0;

}

int main(int argc, char ** argv)
{

	sn_uint32 uProcessID;
	sn_uint32 uSrcItemId[1];
	sn_notify Notify;
	sn_uint32 uSize32 = 4;
	
	printf("This example script adds a custom menu item into a source view context menu which will print current Event Queue information into this view\n");

	AddMenuItem("Get Event Queue List and Info", &uSrcItemId[0], "DBGSourceView");
	RequestNotification(NT_CUSTMENU);
	RequestNotification(NT_KEYDOWN);        //  Allows exit from script
//	TextClear();

while (1)
	{
		GetNotification(NT_ANY, &Notify);
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[0])
		{
			TextClear();
			PS3UpdateKernelInfo();
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			GetEventQueueList(uProcessID);
		}
	}

return 0;
}