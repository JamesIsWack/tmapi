/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "SyncCommand.h"

TargetCommand* SyncCommandFactory(void)
{
	return new SyncCommand();
}

SyncCommand::SyncCommand()
: TargetCommand()
, m_bWaitForTransfers(false)
, m_bForceSync(false)
, m_Direction (TX_DIRECTION_UPLOAD)
{

}

SyncCommand::~SyncCommand()
{

}

bool SyncCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	StandardOption dl("dl", "download");
	StandardOption fs("fs", "force-sync");
	StandardOption wt("wt", "wait");

	MultiArgOption<std::string> fl("fl","files", true);

	SingleArgOption<std::string> dp("dp", "dest-path", "", true);

	m_cmdLineHandler.AddArgument(dl);
	m_cmdLineHandler.AddArgument(fs);
	m_cmdLineHandler.AddArgument(wt);

	m_cmdLineHandler.Parse(arguments);

	m_bForceSync = fs.IsSet();
	m_bWaitForTransfers = wt.IsSet();
	m_dstPath = dp.GetValue();

	m_Direction = dl.IsSet()?TX_DIRECTION_DOWNLOAD:TX_DIRECTION_UPLOAD;

	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();

	if (remainingArgs.size() > 1)
	{
		m_dstPath = remainingArgs.back();
		remainingArgs.erase(remainingArgs.end() - 1); // remove dest path

		std::vector<std::string>::const_reverse_iterator it = remainingArgs.rbegin();
		for (; it != remainingArgs.rend(); ++it)
		{
			m_srcFiles.push_back(*it);
		}
	}
	return true;
}

int SyncCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	// Determine the type of transfer
	TransferType TxType = IdentifyTransferType();

	if (m_bWaitForTransfers)
		if (!SetupFileEvents())
			return GetErrorCodeOnError();

	if (m_bForceSync)
		PrintMessage(ML_INFO, L"Doing a force transfer\n");

	switch (TxType)
	{
	case TX_TYPE_UNKNOWN:
		break;
	case TX_TYPE_FILE_TO_FILE:
		{
			if (!PerformFileToFile())
				return GetErrorCodeOnError();
		}
		break;
	case TX_TYPE_DIRECTORY_TO_DIRECTORY:
		{
			if (!PerformDirectoryToDirectory())
				return GetErrorCodeOnError();
		}
		break;
	case TX_TYPE_FILE_TO_DIRECTORY:
		{
			if (!PerformFileToDirectory())
				return GetErrorCodeOnError();
		}
		break;
	}

	if (m_bWaitForTransfers)
	{
		// Go into event processing loop.
		if (ProcessEvents() == false)
			return GetErrorCodeOnError();
	}

	return m_exitCode;
}

SyncCommand::TransferType SyncCommand::IdentifyTransferType()
{
	//!!! Needs more thought

	if (m_Direction==TX_DIRECTION_DOWNLOAD)
	{
		SNPS3DirEntry Entry;
		BOOL bSrcIsDir = FALSE;
		BOOL bDstIsDir = FALSE;

		if (SN_SUCCEEDED(SNPS3StatTargetFile(m_targetId, m_srcFiles[0].c_str(),&Entry)))
		{
			if (Entry.Mode & SNPS3_DIRENT_TYPE_DIRECTORY)
				bSrcIsDir = TRUE;
		}
		else
		{
			// No stat, invalid source specified
			return TX_TYPE_UNKNOWN;
		}

		DWORD dwRet = ::GetFileAttributesW(UTF8ToWChar(m_dstPath.c_str()).c_str());
		if (INVALID_FILE_ATTRIBUTES == dwRet)
		{
			return TX_TYPE_UNKNOWN;
		}

		if (dwRet & FILE_ATTRIBUTE_DIRECTORY)
		{
			bDstIsDir = TRUE;
		}
		
		if (bSrcIsDir && bDstIsDir)
		{
			return TX_TYPE_DIRECTORY_TO_DIRECTORY;
		}
		else if (!bSrcIsDir && bDstIsDir)
		{
			return TX_TYPE_FILE_TO_DIRECTORY;
		}
		else if (!bSrcIsDir && !bDstIsDir && m_srcFiles.size() == 1)
		{
			return TX_TYPE_FILE_TO_FILE;
		}
		return TX_TYPE_UNKNOWN;
	}
	else
	{
		// Directory to directory
		if (m_srcFiles.size() == 1)
		{
			DWORD dwRet = ::GetFileAttributesW(UTF8ToWChar(m_srcFiles[0]).c_str());
			if ((dwRet != -1) && (dwRet & FILE_ATTRIBUTE_DIRECTORY))
			{
				return TX_TYPE_DIRECTORY_TO_DIRECTORY;
			}
		}

		SNPS3DirEntry Entry;
		if (SN_SUCCEEDED( SNPS3StatTargetFile(m_targetId, m_dstPath.c_str(), &Entry) ))
		{
			if (Entry.Mode & SNPS3_DIRENT_TYPE_DIRECTORY)
				return TX_TYPE_FILE_TO_DIRECTORY;
		}
	}

	if (m_srcFiles.size() == 1)
		return TX_TYPE_FILE_TO_FILE;

	return TX_TYPE_UNKNOWN;
}

bool SyncCommand::PerformFileToFile()
{
	_ASSERT(m_srcFiles.size() == 1);
	if (m_srcFiles.size() == 0)
		return false;

	if (m_Direction == TX_DIRECTION_DOWNLOAD)
	{
		UINT32 txId = m_bForceSync ? TXID_FORCE_FLAG : 0;
		if (SN_SUCCEEDED( SNPS3DownloadFile(m_targetId, m_srcFiles[0].c_str(), m_dstPath.c_str(), &txId) ))
			m_PendingTransfers.push_back(txId);
	}
	else
	{
		UINT32 txId = m_bForceSync ? TXID_FORCE_FLAG : 0;
		if (SN_SUCCEEDED( SNPS3UploadFile(m_targetId, m_srcFiles[0].c_str(), m_dstPath.c_str(), &txId) ))
			m_PendingTransfers.push_back(txId);
	}

	return true;
}

bool SyncCommand::PerformDirectoryToDirectory()
{
	_ASSERT(m_srcFiles.size() == 1);
	if (m_srcFiles.size() == 0)
		return false;

	if (m_Direction == TX_DIRECTION_DOWNLOAD)
	{
		UINT32 txId = m_bForceSync ? TXID_FORCE_FLAG : 0;
		if (SN_SUCCEEDED( SNPS3DownloadDirectory(m_targetId, m_srcFiles[0].c_str(), m_dstPath.c_str(), &txId) ))
			m_PendingTransfers.push_back(txId);
	}
	else
	{
		UINT32 txId = m_bForceSync ? TXID_FORCE_FLAG : 0;
		if (SN_SUCCEEDED( SNPS3UploadDirectory(m_targetId, m_srcFiles[0].c_str(), m_dstPath.c_str(), &txId) ))
			m_PendingTransfers.push_back(txId);
	}


	PrintMessage(ML_INFO, L"Started Directory to Directory transfer\n");

	return true;
}

std::string	SyncCommand::GetFileNameWithExt(const char* szPath)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szName[_MAX_FNAME];
	char szExt[_MAX_EXT];

	_splitpath_s(szPath,szDrive,szDir,szName,szExt);

	std::string strResult = szName;
	strResult += szExt;
	
	return strResult;
}

bool SyncCommand::PerformFileToDirectory()
{
	_ASSERT(m_srcFiles.size() != 0);

	StringIterator it = m_srcFiles.begin();
	for (; it != m_srcFiles.end(); ++it)
	{
		std::string& thisFile = *it;
		WCHAR szFullPath[_MAX_PATH];
		WCHAR* pszFileName = NULL;

		if (m_Direction == TX_DIRECTION_DOWNLOAD)
		{
			std::string strDestination = m_dstPath;
			strDestination += '/';
			strDestination += GetFileNameWithExt(thisFile.c_str());

			UINT32 txId = m_bForceSync ? TXID_FORCE_FLAG : 0;
			if (SN_SUCCEEDED( SNPS3DownloadFile(m_targetId, thisFile.c_str(), strDestination.c_str(), &txId)))
				m_PendingTransfers.push_back(txId);
		}
		else
		{
			if (::GetFullPathName(UTF8ToWChar(thisFile).c_str(), _MAX_PATH, szFullPath, &pszFileName))
			{
				std::wstring szDestination = UTF8ToWChar(m_dstPath);
				szDestination += '/';
				szDestination += pszFileName;

				UINT32 txId = m_bForceSync ? TXID_FORCE_FLAG : 0;
				if (SN_SUCCEEDED( SNPS3UploadFile(m_targetId, thisFile.c_str(), WCharToUTF8(szDestination).c_str(), &txId) ))
					m_PendingTransfers.push_back(txId);
			}
		}
	}

	PrintMessage(ML_INFO, L"Started File to Directory transfer\n");
	return true;
}

bool SyncCommand::SetupFileEvents()
{
	SNRESULT snr;
	if (SN_FAILED( snr = SNPS3RegisterFTPEventHandler(m_targetId, SyncCommand::TransferEventCallback, NULL) ))
	{
		PrintError(snr, UTF8ToWChar(std::string("Error - Failed to register for target events")).c_str());
		return false;
	}

	return true;
}

void SyncCommand::ProcessTransferEvent(HTARGET hTarget, UINT uLength, BYTE* pData)
{
	TMAPI_FT_NOTIFICATION* pNotification = (TMAPI_FT_NOTIFICATION*) pData;
	UINT uCount = uLength / sizeof(TMAPI_FT_NOTIFICATION);

	for (UINT i = 0 ; i < uCount ; ++i, ++pNotification)
	{
		switch (pNotification->m_Type)
		{
		case TMAPI_FT_ERROR:
		case TMAPI_FT_FINISH:
		case TMAPI_FT_SKIPPED:
		case TMAPI_FT_CANCELLED:
			CompleteTransfer(pNotification->m_TransferID);
			break;
		case TMAPI_FT_PROGRESS:
		case TMAPI_FT_REFRESH_LIST:
			break;
		default:
			_ASSERTE(FALSE);
			break;
		}
	}
}

void __stdcall SyncCommand::TransferEventCallback(HTARGET hTarget, UINT uEventType, UINT uEventParam, SNRESULT snr,
												  UINT uLength, BYTE* pEventData, void* pUserData)
{
	if (SN_FAILED( snr ))
		return;

	switch (uEventType)
	{
	case SN_EVENT_FTP:
		((SyncCommand*)ms_TargetCommandObj)->ProcessTransferEvent(hTarget, uLength, pEventData);
		break;
	}
}

void SyncCommand::CompleteTransfer(UINT id)
{
	if (m_PendingTransfers.front() == id) // Should always be the case
	{
		m_PendingTransfers.pop_front();
	}
	else
	{
		m_PendingTransfers.erase(
			std::remove(m_PendingTransfers.begin(),m_PendingTransfers.end(), id),
			m_PendingTransfers.end()
			);
	}

	if (m_PendingTransfers.size() == 0)
		PrintMessage(ML_INFO, L"Transfer completed\n");
}

bool SyncCommand::ProcessEvents()
{
	SNRESULT snr = ClearPendingMessages();

	while (SN_SUCCEEDED(snr) && !m_PendingTransfers.empty())
	{
		snr = ClearPendingMessages();
	}

	return true;
}

SNRESULT SyncCommand::ClearPendingMessages(void)
{
	SNRESULT snr = SN_S_OK;

	do
	{
		snr = SNPS3Kick();

	} while (snr == SN_S_OK);

	return snr;
}

void SyncCommand::DisplayUsageHelp() const
{
	std::cout << "The sync command allows you to upload files to a target" << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl sync <options> <file1> [<file2> [<file3> [...]]] <destination>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -dl" << "\t\t" << "Download files. Default is upload if this flag not present." << std::endl;
	std::cout << "  -fs" << "\t\t" << "Force synchronization (skip timestamp check)" << std::endl;
	std::cout << "  -wt" << "\t\t" << "Wait for the transfers to complete" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
