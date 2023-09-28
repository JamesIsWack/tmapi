// WinPipeServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwBufferSize = 1024;
	BYTE Buffer[1024];
	DWORD dwRead;

	HANDLE hPipe = ::CreateNamedPipe("\\\\.\\pipe\\NamedPipeExample.pipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE|PIPE_WAIT, 1, 1024, 1024, 500/*ms*/, NULL);

	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		printf("CreatePipe failed"); 
		return EXIT_FAILURE;
	}

	// Wait for the client to connect; if it succeeds, 
	// the function returns a nonzero value. If the function
	// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

	BOOL bOK = ::ConnectNamedPipe(hPipe, NULL) || (::GetLastError() == ERROR_PIPE_CONNECTED);

	while (bOK)
	{
		bOK = ::ReadFile(hPipe, &Buffer, 1024, &dwRead, NULL);

		// Just print out what we've received.
		if (bOK)
		{
			printf("%s", (char*)Buffer);
		}
	}

	::CloseHandle(hPipe);

	return EXIT_SUCCESS;
}
