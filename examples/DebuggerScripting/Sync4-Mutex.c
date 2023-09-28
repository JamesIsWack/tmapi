//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: 
//  Illustrates use of PS3GetMutexInfo and PS3GetMutexList
// 
//
//****************************************************************************
sn_uint32 GetMutexList(sn_uint32 uProcessID)
{

	sn_uint32 uCount;
	sn_uint32* pBuffer = NULL;
	sn_uint32 i, j;
	sn_uint64* pWaitingThreads;
			
	SNPS3_MUTEX_INFO* pInfo;
	sn_uint32 IDBuffsize = 10 * sizeof(sn_uint64);

	if (SN_FAILED(PS3GetMutexList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	
	pBuffer = (sn_uint32*)malloc(uCount * sizeof(sn_uint32));
		
	if (SN_FAILED(PS3GetMutexList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	////////////////////////////////////////////////////////////
	printf("List of Mutex IDs:\n");
	
	pInfo = (SNPS3_MUTEX_INFO*)malloc(sizeof(SNPS3_MUTEX_INFO));
	pInfo->pWaitThreadIDs = (sn_uint64*)malloc(10 * sizeof(sn_uint64));

	for (i = 0; i < uCount ; i++)
	{
		if (SN_FAILED(PS3GetMutexInfo(uProcessID, pBuffer[i], pInfo, pInfo->pWaitThreadIDs,  &IDBuffsize)))
			continue;

		printf("\nID 0x%x, AttrProtocol 0x%x AttrRecursive 0x%x AttrPshared 0x%x AttrAdaptive 0x%x Key Flags 0x%x name %s OwnerThread ID 0x%x, LockCounter 0x%x CondID 0x%x CondRefCounter 0x%x", 
			pBuffer[i], pInfo->Attribute.uAttrProtocol, pInfo->Attribute.uAttrRecursive, pInfo->Attribute.uAttrPshared,  pInfo->Attribute.uAttrAdaptive, pInfo->Attribute.uFlags, pInfo->Attribute.aName, pInfo->uLockCounter, pInfo->uCondRefCounter);
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
	
	printf("This example script adds a custom menu item into a source view context menu which will print current mutex information into this view\n");
	AddMenuItem("Get Mutex List and Info", &uSrcItemId[0], "DBGSourceView");
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
			GetMutexList(uProcessID);
		}
	}

return 0;
}
