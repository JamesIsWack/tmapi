/****************************************************************************/
// Copyright SN Systems Ltd 2011
//
// String server which creates a named pipe and waits for a connection
// from a debugger script, waits for and processes any string requests.
//****************************************************************************/

#include <stdio.h>
#include <Windows.h>

#define BUF_SIZE 256
#define TIME_OUT 3000
#define NUM_INSTANCES 255

HANDLE hPipe; 

int CreateAndConnect(void)
{
	DWORD dwError;
	char pszPipeName[] = "\\\\.\\pipe\\MyNamedPipe";
	hPipe = CreateNamedPipe(pszPipeName,  PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, PIPE_NOWAIT, NUM_INSTANCES, BUF_SIZE, BUF_SIZE, TIME_OUT, NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
	{ 
		printf("Error creating pipe: %d \n", GetLastError());
		return 0;
	}

	printf("Connect to Named Pipe\n");

	while (!ConnectNamedPipe(hPipe, NULL))
	{
		dwError = GetLastError();
		if (dwError == ERROR_PIPE_CONNECTED)
		{
			printf("Success! There is a process connected\n");
			return 1;
		}

		if (dwError != ERROR_PIPE_LISTENING)
		{
			printf("Connect Error: 0x%x\n", dwError);
			return 0;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	DWORD nBytesRead, dwError;
	if(!CreateAndConnect())
		return 0;

	while(1)
	{
		char pInBuffer[BUF_SIZE];
		char pOutBuffer[BUF_SIZE];

		while(!ReadFile(hPipe, pInBuffer, BUF_SIZE, &nBytesRead, NULL))
		{
			dwError = GetLastError();
			if (dwError == ERROR_BROKEN_PIPE)
			{
				if(!CreateAndConnect())
					return 0;
			}
		}

		printf("\nClient request, ID: %s", pInBuffer);

		int nStringID = atoi(pInBuffer);

		switch(nStringID) 
		{
		case 0:
			_snprintf_s(pOutBuffer, BUF_SIZE, "%s", "This is ZERO!");
			break;
		case 1:
			_snprintf_s(pOutBuffer, BUF_SIZE, "%s", "Numero Uno");
			break;
		case 2:
			_snprintf_s(pOutBuffer, BUF_SIZE, "%s", "2 Bananas");
			break;
		case 3:
			_snprintf_s(pOutBuffer, BUF_SIZE, "%s", "Three Elephants");
			break;
		default:
			_snprintf_s(pOutBuffer, BUF_SIZE, "%s", "Huh?");
		}

		printf("\nResponding\n");

		DWORD dwBytesWritten;
		if (!WriteFile(hPipe, pOutBuffer, BUF_SIZE, &dwBytesWritten, NULL))
		{
			dwError = GetLastError();
			printf("\nWriting Response: 0x%x\n", dwError);
			return 0;
		}
	}

	system("pause");
	return 1;
}