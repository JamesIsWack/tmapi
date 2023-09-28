//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: 
//  Illustrates use of PS3GetLightWeightMutexList and PS3GetLightWeightMutexInfo to detect deadlocks in code. 
// This could easily be extended to cover other synchronisation primatives using the OwnerNode and WaitingNode structures
// 
//
//****************************************************************************
typedef struct OwnerNode
{
	sn_uint32 uOThreadID;
	sn_uint32 LWMutexID;
	BOOL bOnPath;
	BOOL bChecked;
}OwnerNode;

typedef struct WaitingNode
{
	sn_uint32 uWThreadID;
	sn_uint32 LWMutexID;
	BOOL bOnPath;
	BOOL bChecked;
}WaitingNode;

const sn_uint32 sizeOwnerArray = 64;
OwnerNode gaOwnerNodes[sizeOwnerArray];

const sn_uint32 sizeWaitArray = 64;
WaitingNode gaWaitingNodes[sizeWaitArray];

sn_uint32 uWaitingNodeCount;
sn_uint32 uOwnerNodeCount;
BOOL bLoopFound;
BOOL WalkOwners(sn_uint32 ThreadID);

void printLoop(void)
{
	sn_uint32 i = 0;
	printf("\n\n*************Loop Found************");
		
	printf("\nWaiting Nodes:");
	for (i = 0; i < uWaitingNodeCount ; i++)
	{
		if (gaWaitingNodes[i].bOnPath == TRUE)
		{
			printf("\nThreadID 0x%x Mutex ID 0x%x", gaWaitingNodes[i].uWThreadID,  gaWaitingNodes[i].LWMutexID);
		}
	}
	printf("\nOwner Nodes:");
	for (i = 0; i < uOwnerNodeCount ; i++)
	{
		if (gaOwnerNodes[i].bOnPath == TRUE)
		{
			printf("\nThreadID 0x%x MutexID 0x%x", gaOwnerNodes[i].uOThreadID,  gaOwnerNodes[i].LWMutexID);
		}
	}
	printf("\n\n***********************************");
}
BOOL WalkWaiters(sn_uint32 LWMutexID)
{
	sn_uint32 i;
	for (i = 0; i < uWaitingNodeCount; i++)
	{
		if (gaWaitingNodes[i].LWMutexID == LWMutexID)
		{
			if (gaWaitingNodes[i].bOnPath == TRUE)
			{
				bLoopFound = TRUE;
				printLoop();
				return TRUE;
			}

			if (gaWaitingNodes[i].bChecked == TRUE)
				continue;

			gaWaitingNodes[i].bOnPath = TRUE;
			gaWaitingNodes[i].bChecked = TRUE;
			WalkOwners(gaWaitingNodes[i].uWThreadID);	
			gaWaitingNodes[i].bOnPath = FALSE;
		}
	}
	return FALSE;
}

BOOL WalkOwners(sn_uint32 ThreadID)
{
	sn_uint32 i;
	
	for (i = 0; i < uOwnerNodeCount; i++)
	{
		if (gaOwnerNodes[i].uOThreadID == ThreadID)
		{
			if (gaOwnerNodes[i].bOnPath == TRUE)
				{
					bLoopFound = TRUE;
					printLoop();
					return TRUE;
				}
			
			if (gaOwnerNodes[i].bChecked == TRUE)
				continue;

			gaOwnerNodes[i].bOnPath = TRUE;
			gaOwnerNodes[i].bChecked = TRUE;
			WalkWaiters(gaOwnerNodes[i].LWMutexID);
			gaOwnerNodes[i].bOnPath = FALSE;
		}
	}
	return FALSE;
}
void WalkTree(void)
{
	sn_uint32 i = 0;
	sn_uint32 uCurWaiter = 0;
	
	for (uCurWaiter = 0; uCurWaiter < uWaitingNodeCount ; uCurWaiter++)
	{
		if (gaWaitingNodes[uCurWaiter].bChecked == TRUE)
			continue;

		gaWaitingNodes[i].bChecked = TRUE;
		gaWaitingNodes[uCurWaiter].bOnPath = TRUE;
		WalkOwners(gaWaitingNodes[uCurWaiter].uWThreadID);
		gaWaitingNodes[uCurWaiter].bOnPath = FALSE;
	}
}
sn_uint32 CheckForDeadLock(sn_uint32 uProcessID)
{

	sn_uint32 uCount;
	sn_uint32* pBuffer = NULL;
	sn_uint32 i, j;
	sn_uint64* pWaitingThreads;
	SNPS3_LWMUTEX_INFO* pInfo;
	sn_uint32 IDBuffsize = 10 * sizeof(sn_uint64);
	WaitingNode tempWnode;
	OwnerNode	tempOnode;

	uWaitingNodeCount = 0;
	uOwnerNodeCount = 0;
	if (SN_FAILED(PS3GetLightWeightMutexList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}
	
	pBuffer = (sn_uint32*)malloc(uCount * sizeof(sn_uint32));
		
	if (SN_FAILED(PS3GetLightWeightMutexList(uProcessID, &uCount, pBuffer)))
	{
		printf("Error: %s", GetLastErrorString());
		return 0;
	}

	pInfo = (SNPS3_LWMUTEX_INFO*)malloc(sizeof(SNPS3_LWMUTEX_INFO));
	pInfo->pWaitThreadIDs = (sn_uint64*)malloc(10 * sizeof(sn_uint64));
	
	for (i = 0; i < uCount ; i++)
	{
		if (SN_FAILED(PS3GetLightWeightMutexInfo(uProcessID, pBuffer[i], pInfo, pInfo->pWaitThreadIDs,  &IDBuffsize)))
			continue;

		tempOnode.uOThreadID = pInfo->uOwnerThreadID.word[0];
		tempOnode.LWMutexID = pInfo->uLWMutexID;
		tempOnode.bOnPath = FALSE;
		tempOnode.bChecked = FALSE;
		
		
		if (uOwnerNodeCount >= sizeOwnerArray)
		{
			printf("\nError: Too many owner nodes, increase sizeOwnerArray\n");
			return 1;
		}
		else
		{
			gaOwnerNodes[uOwnerNodeCount++] = tempOnode;
		}
		
		pWaitingThreads = pInfo->pWaitThreadIDs;
		for (j = 0; j < pInfo->uWaitThreadsNum; j++)
		{
			tempWnode.LWMutexID = pInfo->uLWMutexID;
			tempWnode.uWThreadID = pWaitingThreads->word[0];
			tempWnode.bOnPath = FALSE;
			tempWnode.bChecked = FALSE;
			
		if (uWaitingNodeCount >= sizeWaitArray)
		{
			printf("\nError: Too many wait nodes, increase sizeWaitArray\n");
			return 1;
		}
		else
		{
			gaWaitingNodes[uWaitingNodeCount++] = tempWnode;
		}

			pWaitingThreads++;
		}
	}
	
	printf("\nLets find a deadlock!\n");
	
	bLoopFound = FALSE;
	WalkTree();
	
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
	
	printf("This example script adds a custom menu item into a source view context menu which will print current deadlock information into this view\n");

	AddMenuItem("Find any LW Mutex deadlocks", &uSrcItemId[0], "DBGSourceView");
	RequestNotification(NT_CUSTMENU);

while (1)
	{
		GetNotification(NT_ANY, &Notify);
		if ((sn_uint32)Notify.pParam1 == uSrcItemId[0])
		{
			TextClear();
			printf("Kernel Update\n");
			PS3UpdateKernelInfo();
			printf("Kernel Update Complete\n");
			GetObjectData(LOWORD(Notify.pParam2), GVD_PROCESS, &uProcessID, uSize32);
			printf("Found Process ID 0x%x", uProcessID);
			CheckForDeadLock(uProcessID);
		}
	}

return 0;
}
