/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "PS3RunCommand.h"

TargetCommand* PS3RunCommandFactory(void)
{
	return new PS3RunCommand();
}

PS3RunCommand::PS3RunCommand()
: ConsoleCommand()
, m_bResetFirst(false)
, m_resetParam(SOFT)
, m_bootParam(SNPS3TM_BOOTP_DEBUG_MODE)
, m_bootMask(SNPS3TM_BOOTP_SYSTEM_MODE)
, m_performPowerCycle(false)
, m_sdkCheck(false)
, m_resetBootParams(false)
, m_modulePriority(SNPS3_DEF_PROCESS_PRI)
, m_debugFlags(0)
, m_processId(0xffffffff)
, m_threadId(0xffffffffffffffff)
, m_bWaitForProcessExit(false)
, m_bShowTM(false)
{

}

PS3RunCommand::~PS3RunCommand()
{

}

bool PS3RunCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!ConsoleCommand::ParseArgs(arguments))
		return false;

	// Reset stuff
	SingleArgOption<std::string> r("r","reset", "soft", false, false);
	SingleArgOption<UINT64> b("b","boot-param", SNPS3TM_BOOTP_DEBUG_MODE);
	SingleArgOption<UINT64> m("m","boot-mask", SNPS3TM_BOOTP_SYSTEM_MODE);
	StandardOption s("s", "power-cycle");
	StandardOption u("u", "reset-boot-params");
	StandardOption nr("nr", "no-reset");
	StandardOption nc("nc", "no-connect");

	b.SetParentDependency(&r);
	m.SetParentDependency(&r);
	s.SetParentDependency(&r);
	u.SetParentDependency(&r);

	m_cmdLineHandler.AddArgument(r);
	m_cmdLineHandler.AddArgument(b);
	m_cmdLineHandler.AddArgument(m);
	m_cmdLineHandler.AddArgument(s);
	m_cmdLineHandler.AddArgument(u);
	m_cmdLineHandler.AddArgument(nr);
	m_cmdLineHandler.AddArgument(nc);

	SingleArgOption<std::string> sfodir("sfodir", "sfo-directory", "");
	StandardOption debug("debug", "enable-module-debugging");
	SingleArgOption<UINT32> pri("pri", "module-priority", SNPS3_DEF_PROCESS_PRI);
	StandardOption w("w", "wait-for-process-exit");

	m_cmdLineHandler.AddArgument(pri);
	m_cmdLineHandler.AddArgument(debug);
	m_cmdLineHandler.AddArgument(sfodir);
	m_cmdLineHandler.AddArgument(w);

	// SDK Check
	StandardOption v("v", "sdk-match");
	m_cmdLineHandler.AddArgument(v);

	StandardOption showTM("show", "showTargetManager");
	m_cmdLineHandler.AddArgument(showTM);

	m_cmdLineHandler.Parse(arguments);

	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
	std::vector<std::string>::iterator it = remainingArgs.begin();
	if (remainingArgs.size() > 0)
	{
		std::string elfPath = *it++;
		DWORD dwRes = 1;
		WCHAR *pchFilePart = NULL;
		WCHAR szFullPath[_MAX_PATH];

		// Get the full path for the ELF.
		if (0 == _strnicmp(elfPath.c_str(), "../dev_bdvd/", strlen("../dev_bdvd/")))
		{
			m_elfPath = elfPath;
		}
		else if (0 == _strnicmp(elfPath.c_str(), "../dev_hdd0/", strlen("../dev_hdd0/")))
		{
			m_elfPath = elfPath;
		}
		else if (0 == _strnicmp(elfPath.c_str(), "../app_home/", strlen("../app_home/")))
		{
			m_elfPath = elfPath;
		}
		else
		{
			dwRes = GetFullPathName(UTF8ToWChar(elfPath).c_str(), _countof(szFullPath), szFullPath, &pchFilePart);
			std::wstring fullPath(szFullPath);
			m_elfPath = WCharToUTF8(fullPath);
		}

		if (dwRes == 0)
			throw ArgumentException("Error - Illegal file specified");

		if (pri.IsPassed())
			m_modulePriority = pri.GetValue();

		if (debug.IsSet())
			m_debugFlags = SNPS3_LOAD_FLAG_ENABLE_DEBUGGING;

		if (sfodir.IsPassed() && !sfodir.GetValue().empty())
		{
			m_debugFlags |= SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USECUSTOMDIR;
			m_debugFlags &= ~SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USEELFDIR;
			m_sfoDirectory = sfodir.GetValue();
		}

		if (w.IsSet())
			m_bWaitForProcessExit = true;

		// Now check for args
		for (it ; it != remainingArgs.end(); ++it)
			m_elfArgs.push_back(*it);
	}

	

	if (r.IsPassed())
	{
		m_bResetFirst = true;
		if (!r.GetValue().empty())
		{
			if (r.GetValue() == "soft")
				m_resetParam = SOFT;
			else if (r.GetValue() == "hard")
				m_resetParam = HARD;
			else if (r.GetValue() == "quick")
				m_resetParam = QUICK;
		}

		if (b.IsPassed())
			m_bootParam = b.GetValue();

		if (m.IsPassed())
			m_bootMask = m.GetValue();

		if (s.IsSet())
			m_performPowerCycle = true;

		if (u.IsSet())
			m_resetBootParams = true;
	}

	if (v.IsSet())
		m_sdkCheck = true;

	if (nr.IsSet())
		m_bResetFirst = false;

	if (showTM.IsSet())
		m_bShowTM = true;

	if (nc.IsSet())
	{
		m_bConnectToTarget = false;
	}

	return true;
}

int PS3RunCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (m_bShowTM)
	{
		STARTUPINFO startupInfo = {0};
		startupInfo.cb = sizeof(startupInfo);
		PROCESS_INFORMATION processInfo = {0};

		std::wstring commandLine = L"ps3tm.exe";

		if (!::CreateProcess(NULL, (LPWSTR)commandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
		{
			std::cout << "Error opening ProDG Target Manager. Error code: " << ::GetLastError() << std::endl;
		}
	}

	if (m_bConsoleIn)
	{
		if (ProcessSendTTY() == false)
			return GetErrorCodeOnError();
	}

	if (m_bConsoleOut)
	{
		if (ProcessRequestTTY() == false)
			return GetErrorCodeOnError();
	}

	if (m_resetBootParams)
	{
		if (!GetResetParameters(m_originalResetParams))
			return GetErrorCodeOnError();
	}

	// Time to do a reset
	if (m_bResetFirst)
	{
		if (SN_FAILED(SNPS3ClearTTYCache(m_targetId)))
			return GetErrorCodeOnError();

		if (!ResetTargetFirst())
			return GetErrorCodeOnError();
	}

	// SDK Check
	if (m_sdkCheck)
	{
		if (!ProcessSDKCheck())
			return GetErrorCodeOnError();
	}

	if (m_bWaitForProcessExit)
	{
		SNRESULT snr;
		if (SN_FAILED( snr = SNPS3RegisterTargetEventHandler(m_targetId, TargetEventCallback, NULL) ))
		{
			PrintError(snr, L"Failed to register for target events");
			return snr;
		}
	}

	if (!m_elfPath.empty())
	{
		if (!ProcessLoadELF())
			return GetErrorCodeOnError();
	}

	if (m_bAbsTimeoutSet)
	{
		_time64(&m_absTimeoutTime);
		m_absTimeoutTime += m_absTimeoutDuration;
	}

	if (m_bWaitForProcessExit || m_bConsoleIn || m_bConsoleOut)
	{
		if (ProcessEvents() == false)
			return GetErrorCodeOnError();
	}

	ClearMessagesBeforeExit();

	if (DisconnectTTYEvents() == false)
		return GetErrorCodeOnError();

	if (m_resetBootParams)
	{
		m_originalResetParams.Boot.uMask = (UINT64) -1;
		if (SetBootParameters(m_originalResetParams) == false)
		{
			PrintMessage(ML_WARN, L"Failed to restore reset parameters\n");
			return GetErrorCodeOnError();
		}
	}

	return m_exitCode;
}

void PS3RunCommand::ProcessTargetEvent(HTARGET hTarget, UINT uDataLen, BYTE *pData)
{

	UINT uDataRemaining = uDataLen;

	while (uDataRemaining)
	{
		SN_EVENT_TARGET_HDR *pHeader = (SN_EVENT_TARGET_HDR *)pData;

		switch (pHeader->uEvent)
		{
		case SN_TGT_EVENT_TARGET_SPECIFIC:
			ProcessTargetSpecificEvent(pHeader->uSize, pData + sizeof(SN_EVENT_TARGET_HDR));
			break;
		}

		uDataRemaining -= pHeader->uSize;
		pData += pHeader->uSize;
	}
}

void PS3RunCommand::ProcessTargetSpecificEvent(UINT uDataLen, BYTE *pData)
{
	SNPS3_DBG_EVENT_HDR *pDbgHeader = (SNPS3_DBG_EVENT_HDR *)pData;
	SNPS3_DBG_EVENT_DATA *pDbgData = (SNPS3_DBG_EVENT_DATA *)(pData + sizeof(SNPS3_DBG_EVENT_HDR));

	switch (pDbgData->uEventType)
	{
	case SNPS3_DBG_EVENT_PROCESS_EXIT:
		{
			UINT64 u64ExitCode = pDbgData->ppu_process_exit.uExitCode;
			UINT32 u32ExitCode = (UINT32)(u64ExitCode >> 32);

			((PS3RunCommand*)ms_TargetCommandObj)->SetExitCode(ntohl(u32ExitCode));
			ms_TargetCommandObj->SetAbortKick();
		}
		break;
	}
}

void __stdcall PS3RunCommand::TargetEventCallback(HTARGET hTarget, UINT uEventType, UINT /*uEvent*/, 
												  SNRESULT snr, UINT uDataLen, BYTE *pData, void* /*pUser*/)
{
	if (SN_FAILED( snr ))
		return;

	switch (uEventType)
	{
	case SN_EVENT_TARGET:
		ProcessTargetEvent(hTarget, uDataLen, pData);
		break;
	}
}

bool PS3RunCommand::ResetTargetFirst()
{
	_ASSERT(m_bResetFirst);
	if (!m_bResetFirst)
		return false;

	SNRESULT snr = SNPS3ResetEx(m_targetId, 
		m_bootParam, m_bootMask,
		static_cast<UINT64>(m_resetParam), (UINT64) -1,
		0, 0);

	if (SN_FAILED( snr ))
	{
		PrintError(snr, L"Failed to reset target");

		if (m_performPowerCycle)
		{
			PrintMessage(ML_INFO, L"Power cycling\n");

			snr = SNPS3PowerOff(m_targetId, 1);

			if (SN_FAILED( snr ))
			{
				PrintError(snr, L"Failed to power off target");
				return false;
			}

			// Need to sleep for a bit as targets report they are off
			// before they actually are
			::Sleep(2000);

			snr = SNPS3PowerOn(m_targetId);

			if (SN_FAILED( snr ))
			{
				PrintError(snr, L"Failed to power on target");
				return false;
			}

			return true;
		}

		return false;
	}

	PrintMessage(ML_INFO, L"Reset target with %I64d (bootparam:0x%I64x, bootmask:0x%I64x)\n", static_cast<UINT64>(m_resetParam), m_bootParam, m_bootMask);

	return true;
}

bool PS3RunCommand::ProcessLoadELF()
{
	size_t numArgs = m_elfArgs.size();

	char   **myArgV = (char**)_alloca((numArgs+1)*sizeof(char*));
	char	*pArg = NULL;
	myArgV[numArgs] = NULL;


	std::string strAux;
	for(size_t i=0; i<numArgs; i++)
	{
		strAux = m_elfArgs[i];
		size_t argLength = strAux.length() +1;
		myArgV[i] = (char*)_alloca(argLength);
		strncpy_s(myArgV[i], argLength, strAux.c_str(), _TRUNCATE);
	}

	if ( (m_debugFlags & SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USECUSTOMDIR)
		&& !(m_debugFlags & SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USEELFDIR) )
	{
		SNRESULT result = SNPS3SetCustomParamSFOMappingDirectory(m_targetId, m_sfoDirectory.c_str());
		if (SN_FAILED(result))
			return false;
	}

	// Download the ELF into target memory.
	SNRESULT snr = SNPS3ProcessLoad(m_targetId, m_modulePriority, m_elfPath.c_str(), 
		(int)numArgs, (const char**)myArgV,
		0, NULL, &m_processId, &m_threadId, m_debugFlags);
	if (SN_FAILED( snr ))
	{
		PrintError(snr, L"Failed to load \"%s\"", UTF8ToWChar(m_elfPath).c_str());
		return false;
	}

	PrintMessage(ML_INFO, L"Loaded %s into memory, ID=0x%X\n", UTF8ToWChar(m_elfPath).c_str(), m_processId);

	return true;
}

bool PS3RunCommand::GetResetParameters(RESET_PARAMETERS& rp)
{
	SNRESULT snr = SNPS3GetResetParameters(m_targetId,
		&rp.Boot.uValue, &rp.Boot.uMask,
		&rp.Reset.uValue, &rp.Reset.uMask,
		&rp.System.uValue, &rp.System.uMask);

	if (SN_FAILED(snr))
	{
		PrintError(snr, L"Failed to get reset parameters");
		return false;
	}

	return true;
}

bool PS3RunCommand::SetBootParameters(RESET_PARAMETERS const& rp)
{
	SNRESULT snr = SNPS3SetBootParameter(m_targetId,
		rp.Boot.uValue, rp.Boot.uMask);

	if (SN_FAILED(snr))
	{
		PrintError(snr, L"Failed to set boot parameters");
		return false;
	}

	return true;
}

bool PS3RunCommand::ProcessSDKCheck(void)
{
	char *pstrAPIVersion = NULL;
	UINT64 uSDKVersion = (UINT64)-1;
	SNRESULT snr = SN_S_OK;

	if (SN_FAILED(snr = SNPS3GetAPIVersion(&pstrAPIVersion) || pstrAPIVersion == NULL))
	{
		PrintError( snr, L"Unable to get API SDK Version");
		return false;		
	}

	if (SN_FAILED(snr = SNPS3GetSDKVersion(m_targetId, &uSDKVersion)))
	{
		PrintError(snr, L"Unable to get target SDK Version");
		return false;		
	}

	//DECR-1000 units will connect in standby, but if the target has not been connected before the sdk is undefined .
	if ((uSDKVersion == (UINT64)-1))
	{
		PrintMessage(ML_INFO, L"Initial attempt to test SDK version failed, attempting to power-on target for more info.\n");

		if (SN_FAILED(snr = SNPS3PowerOn(m_targetId)))
		{
			PrintError(snr, L"Unable to power target on.  Please power on manually and retry.\n");
			return false;
		}

		if (SN_FAILED(snr = SNPS3GetSDKVersion(m_targetId, &uSDKVersion)))
		{
			PrintError(snr, L"Unable to get target SDK Version");
			return false;		
		}

		if ((uSDKVersion == (UINT64)-1))
		{
			PrintError(SN_E_COMMS_ERR, L"Unable to get target SDK Version");
			return false;
		}

	}

	UINT8 uSDKMajorVersion = static_cast<UINT8>(SNPS3_GET_SDK_VER_MAJOR(uSDKVersion));
	UINT8 uSDKMinorVersion = static_cast<UINT8>(SNPS3_GET_SDK_VER_MINOR(uSDKVersion));

	//minor version number is stored as an individual number, so in 2.80, the minor will be 8,instead of 80
	//need to check for this otherwise the minor value check will fail below..
	if (uSDKMinorVersion < 10)
		uSDKMinorVersion *= 10;

	//pstrAPIVersion format - major/minor are in the same section, separated from rest of the info by a comma..		
	UINT16 uAPIMajorMinorVersion = atoi(pstrAPIVersion);
	if (errno == ERANGE) //atoi overflow error
	{
		PrintError (SN_E_BAD_PARAM, L"Overflow of API SDK version: %d", uAPIMajorMinorVersion);
		return false;
	}

	UINT8 uAPIMajorVesion = uAPIMajorMinorVersion / 100;
	UINT8 uAPIMinorVesion = uAPIMajorMinorVersion - (100 * uAPIMajorVesion);

	if (!(uSDKMajorVersion == uAPIMajorVesion && uSDKMinorVersion == uAPIMinorVesion))
	{
		PrintMessage(ML_ERROR, L"SDK mismatch. Target SDK:%d.%02d, ProDG SDK:%d.%02d", 
			uSDKMajorVersion, uSDKMinorVersion, 
			uAPIMajorVesion , uAPIMinorVesion);
		return false;						
	}

	PrintMessage(ML_INFO, L"Target and API SDK's match: %d.%02d\n", uSDKMajorVersion, uSDKMinorVersion);
	return true;
}

bool PS3RunCommand::WaitOnEvents()
{
	return ConsoleCommand::WaitOnEvents() || m_bWaitForProcessExit;
}

void PS3RunCommand::DisplayUsageHelp() const
{
	std::cout << "The run command allows you to perform the PS3Run tool commands" << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl run <options> [<file> [<args>]]" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;

	std::cout << "  -r <type>" << "\t" << "Reset target first. Values: 'hard|soft|quick'. No value = hard" << std::endl;
	std::cout << "   -b <hex>" << "\t" << "Use <hex> as boot parameter for reset" << std::endl;
	std::cout << "   -m <hex>" << "\t" << "Use <hex> as boot mask for reset" << std::endl;
	std::cout << "   -s" << "\t\t" << "Perform power off and power on if reset fails" << std::endl;
	std::cout << "   -u" << "\t\t" << "Restore boot parameters (if changed) to their original values" << std::endl;
	std::cout << "  -nr" << "\t\t" << "No reset (default)" << std::endl;
	std::cout << "  -show" << "\t" << "Show ProDG Target Manager on execute" << std::endl;
	std::cout << "  -sfodir <dir>" << " Use param.sfo in <dir> when loading Elf." << std::endl;
	std::cout << "  -debug" << "\t" << "Enable module debugging" << std::endl;
	std::cout << "  -pri <val>" << "\t" << "Set priority of module to be loaded to <val>" << std::endl;
	std::cout << "  -w" << "\t\t" << "Wait for process to exit. PS3Ctrl exits on receipt" << std::endl;
	std::cout << "  -v" << "\t\t" << "API and target SDK must match before ELF file will run" << std::endl;
	std::cout << "  -c <channel>" << "\t" << "Waits and sends console input. (Use -i <file>) for file input" << std::endl;
	std::cout << "   -i <file>" << "\t" << "Import Console Input text from file (use '-i stdin' for stdin)" << std::endl;
	std::cout << "  -p <channels>" << "\t" << "Prints output for specified channels. Empty value=All Channels" << std::endl;
	std::cout << "     channels:" << "\t" << "ALL, TM, KERNEL, PPU, PPU (STDERR), SPU, USER 1, USER 2..13" << std::endl;
	std::cout << "   " << "\t\t" << "(See default Console Output tabs in TM)" << std::endl;
	std::cout << "  -a <text>" << "\t" << "Abort if console text found in output" << std::endl;
	std::cout << "  -e <text>" << "\t" << "Exit if console text found in output" << std::endl;
	std::cout << "  -y <timeout>" << "\t" << "Terminate after <timeout> seconds" << std::endl;
	std::cout << "  -z <timeout>" << "\t" << "Terminate after <timeout> seconds without event" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
