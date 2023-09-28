/****************************************************************************/
//
// Copyright SN Systems Ltd 2011
//
// Description: This example waits for NT_TTY notifcations; verifies the 
// updated stream  is 'Kernel' and then reads the TTY data passed as part of
// a TTY_PARAM_INFO structure with the notification and parses it for 
// 'Error'; printing out the remainder of the subsequent line to a file.
// Pushing 'E' will open up the file containing the errors and attempt to send
// an email both using ScriptExecute.
//19/10/2007 - AndyG - Updated to account for extended ScriptExecute Parameters
/****************************************************************************/

sn_uint32 ParseTTY(char* TTY, sn_uint32 uLength, const char* pszSearch, FILE* pFile)
{

	sn_uint32 j;
	sn_uint32 uFound = 0;
	sn_uint32 uStrLen = 0;
	sn_uint32 uFoundStart = 0;

	char *Result;
	uStrLen = strlen(pszSearch);

	for (j=0;j<uLength;j++)
	{
		if (TTY[j] == pszSearch[uFound])
		{
			if (uFound == 0)
			{
				uFoundStart = j;
			}
			uFound++;
		}
		else
		{
			if (uFound != 0)
				j = uFoundStart;
			uFound = 0;
			uFoundStart = 0;
		}
		if (uFound >= uStrLen)
		{
			Result = &TTY[uFoundStart];
			while (*Result != '\n')
			{
				fputc((char)*Result, pFile);
				Result++;
			}
			fputc('\n', pFile);
		}
	}
	return 0;
}

int main()
{
	FILE *fp;
	char* pData;
	sn_notify Notify;
	sn_uint32 i;
	
	char filePath[1024];
	char fileName[1024];
	char cmd[1024];
	
	TTY_PARAM_INFO* pTTYParam;
	RequestNotification(NT_KEYDOWN);        
	RequestNotification(NT_TTY);
	


	printf("This example will parse input TTY for 'Error', press 'E' in a script view to send email with all errors\n");
	/* open file for output */
	FileSelectionDialog(filePath, fileName);
    fp = fopen(filePath, "w+");
    if (NULL == fp) {
        printf("Cannot open file\n");
    }

	while(1)
	{
		GetNotification(NT_ANY, &Notify);   
		switch (Notify.eNotifyType)
		{
		case NT_TTY:
			printf("receiving tty\n");
			pData = (char*)(Notify.pParam1);
			pTTYParam = (TTY_PARAM_INFO*)Notify.pParam2;
			if (strcmp(pTTYParam->ChannelName, "Kernel")  >= 0)
				ParseTTY(pData, pTTYParam->uLen, "Error", fp);
			break;
		case NT_KEYDOWN:
			if ((sn_uint32)Notify.pParam1 == 'E')
			{
				PS3StopAllProcesses();
				fclose(fp);
				sprintf(&cmd[0], "mailto:user@snsys.com?subject=GameError&body=See %s", filePath);
				ScriptExecute("open", &cmd[0], "", "", SW_SHOW);
			}
			if ((sn_uint32)Notify.pParam1 == 'X')
			{
				printf("Exiting\n");
				fclose(fp);
				return 0;
			}
			break;
		default:
			printf("Type: %d", Notify.eNotifyType);
			break;
		}
	}
	return 0;
}
