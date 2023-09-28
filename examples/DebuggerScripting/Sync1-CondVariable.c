//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: 
//  Illustrates use of PS3GetConditionalVariableInfo and PS3GetConditionalVariableList
// 
//
//****************************************************************************
sn_uint32 GetConditionalVariableList(sn_uint32 uProcessID)
{

	sn_uint32 uCount;
	sn_uint32* pBuffer = NULL;
	sn_uint32 i, j;
	sn_uint64* pWaitingThreads;
			
	SNPS3_CONDITIONAL_INFO* pInfo;
	sn_uint32 IDBuffsize = 10 * sizeof(sn_uint64);

	if (SN_FAILED(PS3GetConditionalVariableList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	
	pBuffer = (sn_uint32*)malloc(uCount * sizeof(sn_uint32));
		
	if (SN_FAILED(PS3GetConditionalVariableList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	////////////////////////////////////////////////////////////
	printf("List of Conditional Variable IDs:\n");
	
	pInfo = (SNPS3_CONDITIONAL_INFO*)malloc(sizeof(SNPS3_CONDITIONAL_INFO));
	pInfo->pWaitThreadIDs = (sn_uint64*)malloc(10 * sizeof(sn_uint64));
	
	for (i = 0; i < uCount ; i++)
	{
		if (SN_FAILED(PS3GetConditionalVariableInfo(uProcessID, pBuffer[i], pInfo, pInfo->pWaitThreadIDs,  &IDBuffsize)))
			continue;

		printf("\nID 0x%x \n\tattr_pshared=0x%x key=0x%x%x flags=0x%x name=%s mutexID=0x%x", pBuffer[i], pInfo->Attribute.uAttrPshared, pInfo->Attribute.uKey.word[0], pInfo->Attribute.uKey.word[1], pInfo->Attribute.uFlags, pInfo->Attribute.aName, pInfo->uMutexID);
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
	
	printf("This example script adds a custom menu item into a source view context menu which will print current conditional variable information into this view\n");

	AddMenuItem("Get Conditional Variable List and Info", &uSrcItemId[0], "DBGSourceView");
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
			GetConditionalVariableList(uProcessID);
		}
	}

return 0;
}
