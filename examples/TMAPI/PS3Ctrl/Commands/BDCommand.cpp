/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "BDCommand.h"

TargetCommand* BDCommandFactory(void)
{
	return new BDCommand();
}

BDCommand::BDCommand()
: TargetCommand(),
m_Command(BDCMD_NONE),
m_bWait(false),
m_Result(SN_S_OK),
m_bFinished(false),
m_uXferid(0)
{

}

BDCommand::~BDCommand()
{

}

bool BDCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<std::string> d("dev", "device", "");
	SingleArgOption<std::string> s("src", "source", "");

	StandardOption c("cp", "copy");
	StandardOption m("mt", "mount");
	StandardOption u("umt", "unmount");
	StandardOption q("fq", "quick-format");
	StandardOption f("fn", "format");
	StandardOption w("w", "wait");
	StandardOption info("info", "info");

	m_cmdLineHandler.AddArgument(w);
	m_cmdLineHandler.AddArgument(c);
	m_cmdLineHandler.AddArgument(m);
	m_cmdLineHandler.AddArgument(u);
	m_cmdLineHandler.AddArgument(q);
	m_cmdLineHandler.AddArgument(f);
	m_cmdLineHandler.AddArgument(d);
	m_cmdLineHandler.AddArgument(s);
	m_cmdLineHandler.AddArgument(info);

	m_cmdLineHandler.Parse(arguments);


	if (w.IsPassed())
	{
		m_bWait = true;
	}

	if (f.IsPassed())
	{
		m_Command =BDCommand::BDCMD_FORMAT;
	}
	else if (q.IsPassed())
	{
		m_Command =BDCommand::BDCMD_QUICKFORMAT;
	}
	else if (u.IsPassed())
	{
		m_Command =BDCommand::BDCMD_UNMOUNT;
	}
	else if (m.IsPassed())
	{
		m_Command =BDCommand::BDCMD_MOUNT;
		if (!d.IsPassed())
		{
			throw ArgumentException("Error - \"-mt\" requires -dev parameter to be set!");
		}
		m_strDevice = d.GetValue();
	}
	else if (info.IsPassed())
	{
		m_Command =BDCommand::BDCMD_QUERY;
		if (!d.IsPassed())
		{
			throw ArgumentException("Error - \"-info\" requires -dev parameter to be set!");
		}
		m_strDevice = d.GetValue();
	}
	else if (c.IsPassed())
	{
		m_Command = BDCommand::BDCMD_COPY;
		if (!s.IsPassed() || !d.IsPassed())
		{
			throw ArgumentException("Error - \"-c\" requires -src and -dev parameters to be set!");
		}
		m_strFile = s.GetValue();
		m_strDevice = d.GetValue();
	}

	m_cmdLineHandler.Reset();

	return true;
}

int BDCommand::Run()
{
	int bRes = TargetCommand::Run();

	if (SN_FAILED(bRes))
		return bRes;

	switch(m_Command)
	{
		case BDCMD_COPY:
			if (!DoCopy())
				return GetErrorCodeOnError();
			break;
		case BDCMD_MOUNT:
			if (!DoMount())
				return GetErrorCodeOnError();
			break;
		case BDCMD_UNMOUNT:
			if (!DoUnmount())
				return GetErrorCodeOnError();
			break;
		case BDCMD_FORMAT:
			if (!DoFormat(SNPS3_BD_FORMAT_TYPE_NORMAL))
				return GetErrorCodeOnError();
			break;
		case BDCMD_QUICKFORMAT:
			if (!DoFormat(SNPS3_BD_FORMAT_TYPE_QUICK))
				return GetErrorCodeOnError();
			break;
		case BDCMD_QUERY:
			if(!DoQuery())
				return GetErrorCodeOnError();
			break;
		default:
			DisplayUsageHelp();
			return PS3CTRL_EXIT_ERROR;
	}

	return m_exitCode;
}

bool BDCommand::DoFormat(UINT32 mode)
{
	SNRESULT snr;

	if (m_bWait)
	{
		if (SN_FAILED(snr = SNPS3RegisterTargetEventHandler(m_targetId,BDCommand::TargetEventCallback,(void*)this)))
		{
			PrintError(snr, L"Error registering for target events!\n");
			return false;
		}
	}

	if (SN_FAILED(snr = SNPS3BDFormat(m_targetId,"/dev_bdemu",mode)))
	{
		PrintError(snr, L"Error formatting the BD emulator!\n");
		return false;
	}

	m_bFinished = false;
	if (m_bWait)
	{
		PrintMessage(ML_INFO, L"Formatting BD emulator, please wait...\n");
		while (!m_bFinished)
		{
			SNPS3Kick();
			Sleep(100);
		}

		if (SN_FAILED(m_Result))
		{
			PrintError(snr, L"Error formatting BD emulator!\n");
			return false;
		}
		PrintMessage(ML_INFO, L"BD emulator format completed.\n");
		return true;
	}
	else
	{
		PrintMessage(ML_INFO, L"Formating of the BD emulator has begun. This will continue in the background, after PS3Ctrl.exe has exited!\n");
	}

	return true;
}

bool BDCommand::DoCopy()
{
	SNRESULT snr;

	if (m_bWait)
	{
		if (SN_FAILED(snr = SNPS3RegisterTargetEventHandler(m_targetId,BDCommand::TargetEventCallback,(void*)this)))
		{
			PrintError(snr, L"Error registering for target events!\n");
			return false;
		}
	}
	// Test the file for reading
	char szFullPath[MAX_PATH+1];

	if (0 == GetFullPathNameA(m_strFile.c_str(),MAX_PATH+1,szFullPath,NULL))
	{
		PrintError(SN_E_FILE_ERROR, L"Cannot resolve the source path!\n");
		return false;
	}

	HANDLE hFile = CreateFileA(szFullPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if ((hFile==NULL) || (hFile==INVALID_HANDLE_VALUE))
	{
		PrintError(SN_E_FILE_ERROR, L"Cannot open the source file!\n");
		return false;
	}

	CloseHandle(hFile);

	if (SN_FAILED(snr = SNPS3BDTransferImage(m_targetId,szFullPath,m_strDevice.c_str(),&m_uXferid)))
	{
		PrintError(snr, L"Error copying disc image!\n");
		return false;
	}

	m_bFinished = false;
	if (m_bWait)
	{
		//Utility should wait until the operation is finished

		PrintMessage(ML_INFO, L"Transferring disc image, please wait...\n");
		while (!m_bFinished)
		{
			SNPS3Kick();
			Sleep(100);
		}

		if (SN_FAILED(m_Result))
		{
			PrintError(snr, L"Error copying disc image!\n");
			return false;
		}
		PrintMessage(ML_INFO, L"Disc image successfully transferred.\n");
		return true;
	}
	else
	{
		PrintMessage(ML_INFO, L"Disc image transfer has begun. This will continue in the background, after PS3Ctrl.exe has exited!\n");
	}

	return true;
}

bool BDCommand::DoMount()
{
	SNRESULT snr;
	PrintMessage(ML_INFO, L"Mounting disc image...\n");
	if (SN_FAILED(snr = SNPS3BDInsert(m_targetId,m_strDevice.c_str())))
	{
		PrintError(snr, L"Error mounting disc image!\n");
		return false;
	}
	PrintMessage(ML_INFO, L"Disc image successfully mounted!\n");
	return true;
}

bool BDCommand::DoUnmount()
{
	SNRESULT snr;
	PrintMessage(ML_INFO, L"Unmounting disc image...\n");
	if (SN_FAILED(snr = SNPS3BDEject(m_targetId,"/dev_bdemu")))
	{
		PrintError(snr, L"Error unmounting disc image!\n");
		return false;
	}
	PrintMessage(ML_INFO, L"Disc image successfully unmounted!\n");
	return true;
}

bool BDCommand::DoQuery()
{
	USES_CONVERSION;

	SNRESULT snr;
	SNPS3_BD_QUERY_DATA BdInfo;
	PrintMessage(ML_INFO, L"Retrieving information...\n");

	if (SN_FAILED(snr = SNPS3BDQuery(m_targetId,m_strDevice.c_str(),&BdInfo)))
	{
		PrintError(snr, L"Query has returned an error!\n");
		return false;
	}
	WCHAR szMessage[512];

	swprintf(szMessage,_countof(szMessage), L"Image index:%d",BdInfo.image_index);
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Selected image index:%d",BdInfo.bdemu_selected_index);
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Image file:%s",A2W(BdInfo.image_file_name));
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Image size:%I64d",BdInfo.image_file_size);
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Memorandum:%s",A2W(BdInfo.image_memorandum));
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Product code:%s",A2W(BdInfo.image_product_code));
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Producer:%s",A2W(BdInfo.image_producer));
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Author:%s",A2W(BdInfo.image_author));
	PrintMessage(ML_INFO, szMessage);
	swprintf(szMessage,_countof(szMessage), L"Date:%8.8s",A2W(BdInfo.image_date));
	PrintMessage(ML_INFO, szMessage);
	switch(BdInfo.image_type)
	{
		case 0:
			PrintMessage(ML_INFO, L"Image Type: N/A");
			break;
		case 1:
			PrintMessage(ML_INFO, L"Image Type: BDSL");
			break;
		case 2:
			PrintMessage(ML_INFO, L"Image Type: BDDL");
			break;
		case 3:
			PrintMessage(ML_INFO, L"Image Type: DVDSL");
			break;
		case 4:
			PrintMessage(ML_INFO, L"Image Type: DVDDL");
			break;
		default:
			PrintMessage(ML_INFO, L"Image Type: Unknown!");
			break;
	}
	swprintf(szMessage,_countof(szMessage), L"Layers:0x%08.8x,0x%08.8x",BdInfo.image_sector_layer0,BdInfo.image_sector_layer1);
	PrintMessage(ML_INFO, szMessage);

	return true;
}

void BDCommand::DisplayUsageHelp() const
{
	std::cout << "The bdemu command allows you to use the BD emulator" << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl bdemu <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -info" << "\t\t" << "Display information about the device (with -dev)" << std::endl;
	std::cout << "  -cp" << "\t\t" << "Copy an ISO image to the target (with -src and -dev)" << std::endl;
	std::cout << "  -mt" << "\t\t" << "Mount the specified emulator device (with -dev)" << std::endl;
	std::cout << "  -umt" << "\t\t" << "Unmount the emulator device" << std::endl;
	std::cout << "  -fq" << "\t\t" << "Quick format the emulator device" << std::endl;
	std::cout << "  -fn" << "\t\t" << "Full format the emulator device" << std::endl;
	std::cout << "  -src" << "\t\t" << "Disc image file" << std::endl;
	std::cout << "  -dev" << "\t\t" << "Destination device (/dev_bdemu/0 etc...)" << std::endl;
	std::cout << "  -w" << "\t\t" << "Wait until the operation finishes (-cp, -fq, -fn only)" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}

void BDCommand::TargetEventCallback(HTARGET target,UINT32 uType,UINT32 uEventSpecific,SNRESULT resultCode,UINT32 udataLength,BYTE* data ,void* pUserdata)	
{
	BDCommand* pThis = static_cast<BDCommand*>(pUserdata);
	SN_EVENT_TARGET_HDR *pDataHdr = reinterpret_cast<SN_EVENT_TARGET_HDR*>(data);

	if (!pThis)
		return;

	if (!pDataHdr)
		return;

	if (pThis->m_targetId != target)
		return;

	if (uType != SN_EVENT_TARGET)
		return;

	SN_TM_EVENT_TGT_BD_DATA *pData = (SN_TM_EVENT_TGT_BD_DATA *)(pDataHdr+1);

	switch (pDataHdr->uEvent)
	{
		case SN_TGT_BD_ISOTRANSFER_FINISHED:
			if (pThis->m_Command == BDCommand::BDCMD_COPY)
			{
				pThis->m_Result = pData->uResult;
				pThis->m_bFinished = true;
			}
			break;
		case SN_TGT_BD_FORMAT_FINISHED:
			if (pThis->m_Command == BDCommand::BDCMD_FORMAT || pThis->m_Command == BDCommand::BDCMD_QUICKFORMAT)
			{
				pThis->m_Result = pData->uResult;
				pThis->m_bFinished = true;
			}
			break;
		case SN_TGT_BD_MOUNT_FINISHED:
			if (pThis->m_Command == BDCommand::BDCMD_MOUNT)
			{
				pThis->m_Result = pData->uResult;
				pThis->m_bFinished = true;
			}
			break;
		case SN_TGT_BD_UNMOUNT_FINISHED:
			if (pThis->m_Command == BDCommand::BDCMD_UNMOUNT)
			{
				pThis->m_Result = pData->uResult;
				pThis->m_bFinished = true;
			}
			break;
	}
}
