//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: 
//  Illustrates use of PS3GetSemaphoreInfo and PS3GetSemaphoreList
// 
//
//****************************************************************************

sn_uint32 GetSemaList(sn_uint32 uProcessID)
{

	sn_uint32 uCount;
	sn_uint32* pBuffer = NULL;
	sn_uint32 i, j;
	sn_uint64* pWaitingThreads;
			
	SNPS3_SEMAPHORE_INFO* pInfo;
	sn_uint32 IDBuffsize = 10 * sizeof(sn_uint64);

	if (SN_FAILED(PS3GetSemaphoreList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	
	pBuffer = (sn_uint32*)malloc(uCount * sizeof(sn_uint32));
		
	if (SN_FAILED(PS3GetSemaphoreList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	////////////////////////////////////////////////////////////
	printf("List of Semaphores:\n");
	
	pInfo = (SNPS3_SEMAPHORE_INFO*)malloc(sizeof(SNPS3_SEMAPHORE_INFO));
	pInfo->pWaitThreadIDs = (sn_uint64*)malloc(10 * sizeof(sn_uint64));

	for (i = 0; i < uCount ; i++)
	{
		if (SN_FAILED(PS3GetSemaphoreInfo(uProcessID, pBuffer[i], pInfo, pInfo->pWaitThreadIDs,  &IDBuffsize)))
			continue;

		printf("\nID 0x%x, AttrProtocol 0x%x AttrPshared 0x%x  Key 0x%x%x Flags 0x%x name %s MaxVal 0x%x CurVal 0x%x", 
			pBuffer[i], pInfo->Attribute.uAttrProtocol, pInfo->Attribute.uAttrPshared,  pInfo->Attribute.uKey.word[0], pInfo->Attribute.uKey.word[1], pInfo->Attribute.uFlags, pInfo->Attribute.aName, pInfo->uMaxVal,  pInfo->uCurVal);
		pWaitingThreads = pInfo->pWaitThreadIDs;

		printf("\n%d threads waiting", pInfo->uWaitThreadsNum);
		for (j = 0; j < pInfo->uWaitThreadsNum; j++)
		{
			printf("\nWaiting thread ID 0x%x%x",  pWaitingThreads->word[1], pWaitingThreads->word[0]);
			pWaitingThreads++;
		}
	}
	
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
	
	printf("This example script adds a custom menu item into a source view context menu which will print current semaphore information into this view\n");

	AddMenuItem("Get Semaphore List and Info", &uSrcItemId[0], "DBGSourceView");
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
			GetSemaList(uProcessID);
		}
	}

return 0;
}
