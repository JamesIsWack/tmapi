/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007  Sony Computer Entertainment Inc. / SN Systems Ltd
//
/////////////////////////////////////////////////////////////////////////
// CustomDeci3.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>
#include <cstdio>
#include <cstdlib>

#include "ps3tmapi.h"

const UINT DEF_PROTOCOL_NUM = 0x1001;
const UINT DEF_PORT_NUM = 0;

HANDLE g_hSemaphore = ::CreateEvent(NULL, FALSE, FALSE, NULL);

void __stdcall Callback(HTARGET /*hTarget*/, SNPS3Protocol protocol, BYTE* pData, UINT uLength, void* /*pUser*/)
{
	printf("Received:\n\t\"%s\"\n", pData);
	SetEvent(g_hSemaphore);
}

bool MakePathToElf(char* pBuffer, int nBufferLen)
{
	char szFileServingRoot[_MAX_PATH];
	if (GetModuleFileNameA(NULL, szFileServingRoot, _MAX_PATH) == 0)
	{
		printf("GetModuleFileName() failed\n");
		return false;
	}

	// Strip off exe name and lowest level folder
	char* pSlash = strrchr(szFileServingRoot, '\\');
	if (pSlash != NULL)
	{
		*pSlash = 0;
		pSlash = strrchr(szFileServingRoot, '\\');
		if (pSlash != NULL)
			*pSlash = 0;
	}

	_snprintf(pBuffer, nBufferLen, "%s\\CustomDeci3.self.ppu.self", szFileServingRoot);
	return true;
}

int main(int argc, char* argv[])
{
	if (SN_FAILED( SNPS3InitTargetComms() ))
		return EXIT_FAILURE;

	HTARGET hTarget = -1;
	if (SN_FAILED( SNPS3PickTarget(NULL, &hTarget) ))
	{
		SNPS3CloseTargetComms();
		return EXIT_FAILURE;
	}

	puts("Target selection - Done");

	if (SN_FAILED( SNPS3Connect(hTarget, NULL) ))
	{
		SNPS3CloseTargetComms();
		return EXIT_FAILURE;
	}

	puts("Target connection - Done");

	if (SN_FAILED( SNPS3Reset(hTarget, SNPS3TM_BOOTP_DEFAULT) ))
	{
		SNPS3CloseTargetComms();
		return EXIT_FAILURE;
	}

	puts("Target reset - Done");

	char szSelf[_MAX_PATH] = {};
	if (!MakePathToElf(&szSelf[0], _MAX_PATH))
	{
		SNPS3CloseTargetComms();
		return EXIT_FAILURE;
	}

	UINT uProcessID = 0;
	UINT64 uThreadID = 0;

	if (SN_FAILED( SNPS3ProcessLoad(hTarget, SNPS3_DEF_PROCESS_PRI, szSelf, 0, NULL, 0, NULL, &uProcessID, &uThreadID, 0) ))
	{
		SNPS3CloseTargetComms();
		return EXIT_FAILURE;
	}

	puts("Process load - Done");

	// Wait for process initialization to complete
	::Sleep(8000);

	SNPS3Protocol Proto = {};

	if (SN_FAILED( SNPS3RegisterCustomProtocolEx(hTarget, DEF_PROTOCOL_NUM, DEF_PORT_NUM, "PS3_LPAR", SNPS3_DEFAULT_PROTO_PRIORITY, &Proto, Callback, NULL) ))
	{
		SNPS3CloseTargetComms();
		return EXIT_FAILURE;
	}

	for (int i = 0 ; i < 3 ; ++i)
	{
		char szMsg[64] = {};
		SNRESULT snr = SN_S_OK;

		_snprintf(szMsg, sizeof(szMsg), "Message %d", i);
		if (SN_FAILED( SNPS3SendCustomProtocolData(hTarget, &Proto, (BYTE*) szMsg, (UINT) strlen(szMsg)+1) ))
		{
			SNPS3UnRegisterCustomProtocol(hTarget, &Proto);
			SNPS3CloseTargetComms();
			return EXIT_FAILURE;
		}

		while (::WaitForSingleObject(g_hSemaphore, 100))
		{
			// Pump windows message queue maybe? 
			SNPS3Kick();
		}
	}

	SNPS3UnRegisterCustomProtocol(hTarget, &Proto);
	SNPS3CloseTargetComms();
	return EXIT_SUCCESS;
}