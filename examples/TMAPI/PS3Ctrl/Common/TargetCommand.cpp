/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include <winsock2.h>
#include <ws2tcpip.h>
#include "TargetCommand.h"

VecCommandType g_Commands;

TargetCommand* TargetCommand::ms_TargetCommandObj;
std::vector<SNPS3TargetInfo*> TargetCommand::ms_Targets;

TargetCommand* TargetCommandFactory(void)
{
	return new TargetCommand();
}


TargetCommand::TargetCommand(bool bConnectToTarget/* = true*/)
: m_bAlwaysDC(false)
, m_bDCifNotConnected(false)
, m_bForceDC(false)
, m_bWasOriginallyConnected(false)
, m_targetId(INVALID_TARGET)
, m_bLogToFile(false)
, m_bSurpressErrorLogging(false)
, m_bAbortKick(false)
, m_abortErrorCode(PS3CTRL_EXIT_OK)
, m_exitCode(PS3CTRL_EXIT_OK)
, m_bConnectToTarget(bConnectToTarget)
, m_bLaunchTargetPicker(false)
{
	ms_TargetCommandObj = this;
}

TargetCommand::~TargetCommand()
{
	OnClose();
}

bool TargetCommand::ParseArgs(std::vector<std::string>& arguments)
{
	SingleArgOption<std::string> targetName("t", "target", "");
	StandardOption pt("pt", "target-picker");

	// Disconnect switches
	StandardOption d("d", "disconnect");
	StandardOption da("da", "always-disconnect");
	StandardOption nd("nd", "never-disconnect");
	StandardOption fd("fd", "force-disconnect");
	StandardOption q("q", "quiet-mode");

	m_cmdLineHandler.AddArgument(targetName);
	m_cmdLineHandler.AddArgument(pt);
	m_cmdLineHandler.AddArgument(d);
	m_cmdLineHandler.AddArgument(da);
	m_cmdLineHandler.AddArgument(nd);
	m_cmdLineHandler.AddArgument(fd);
	m_cmdLineHandler.AddArgument(q);

	MultiArgOption<std::string> add("add", "add-targets");
	MultiArgOption<std::string> del("del", "delete-targets");
	SingleArgOption<std::string> setDefault("sd", "set-default-target", "");

	m_cmdLineHandler.AddArgument(add);
	m_cmdLineHandler.AddArgument(del);
	m_cmdLineHandler.AddArgument(setDefault);

	// File serving
	SingleArgOption<std::string> f("f", "file-serving-path", "");
	SingleArgOption<std::string> h("h", "home-directory-path", "");
	m_cmdLineHandler.AddArgument(f);
	m_cmdLineHandler.AddArgument(h);

	// Logging to file
	SingleArgOption<std::string> l("l", "log", "");
	m_cmdLineHandler.AddArgument(l);

	if ((arguments.size() > 1 && (arguments[1] == "help" || arguments[1] == "-help")) 
		|| (arguments.size() > 0 && (arguments[0] == "help" || arguments[0] == "-help")))
	{
		ShowUsage();
		return false;
	}

	// Do the parsing of command line
	m_cmdLineHandler.Parse(arguments);

	m_targetName = targetName.GetValue();
	m_bLaunchTargetPicker = pt.IsSet();
	m_bDCifNotConnected = d.IsSet();
	m_bAlwaysDC = da.IsSet();
	if (nd.IsSet())
	{
		m_bDCifNotConnected = false;
		m_bAlwaysDC = false;
	}
	m_bForceDC = fd.IsSet();
	m_bSurpressErrorLogging = q.IsSet();

	if (add.IsPassed() && add.GetValues().size() > 0)
	{
		std::vector<std::string>& addTargetsVect = add.GetValues();
		StringIterator it = addTargetsVect.begin();
		for (; it != addTargetsVect.end(); ++it)
		{
			std::string& thisTarget = *it;

			TARGET_ADD_DATA data;
			data.Port = 0;
			data.TargetType = "PS3_DEH_TCP";

			if (ExtractTargetDetails(thisTarget, data))
			{
				m_addTargetsVect.push_back(data);
			}
			else
			{
				PrintMessage(ML_ERROR, L"Error parsing target data: %s\n", UTF8ToWChar(thisTarget).c_str());
			}
		}
	}

	if (del.IsPassed() && del.GetValues().size() > 0)
		m_deleteTargetsNames = del.GetValues();

	if (setDefault.IsPassed() && !setDefault.GetValue().empty())
	{
		m_defaultTarget = setDefault.GetValue();
	}

	if (f.IsPassed())
		m_fileServingPath = f.GetValue();
	if (h.IsPassed())
		m_homeDirectoryPath = h.GetValue();

	if (l.IsPassed() && !l.GetValue().empty())
	{
		m_logFilePath = l.GetValue();
		m_bLogToFile = true;
	}

	m_cmdLineHandler.Reset();

	return true;
}

int TargetCommand::InitTargetComms()
{
	SNRESULT snr = SN_S_OK;

	// Initialise PS3tmapi.
	if (SN_FAILED(snr = SNPS3InitTargetComms()))
	{
		PrintError(snr, L"Failed to initialize PS3TM SDK");
		snr =  PS3CTRL_EXIT_ERROR;
	}

	return snr;
}

void TargetCommand::OnClose()
{
	// Do post stuff like disconnecting from target;
	if (m_bAlwaysDC || (!m_bWasOriginallyConnected && m_bDCifNotConnected))
		if (m_targetId != INVALID_TARGET)
			SNPS3Disconnect(m_targetId);

	SNPS3CloseTargetComms();
}

void TargetCommand::PrintError(SNRESULT snr, const WCHAR* pszMessage, ...)
{
	if (ms_TargetCommandObj)
		ms_TargetCommandObj->SetExitCode(snr);

	const char* pszError = NULL;
	SNPS3TranslateError(snr, &pszError);
	if (pszError == NULL)
		pszError = "Unknown";

	va_list	args;
	va_start(args, pszMessage);

	WCHAR szFormattedMsg[4 * 1024];
	_vsnwprintf(szFormattedMsg, _countof(szFormattedMsg), pszMessage, args);
	szFormattedMsg[_countof(szFormattedMsg)-1] = '\0';

	PrintMessage(ML_ERROR, L"%s: %ld (%s)\n", szFormattedMsg, snr, CUTF8ToWChar(pszError).c_str(), args);

	UINT32 qualifier = 0;
	char* szErrorQual = NULL;
	if (SN_SUCCEEDED(SNPS3GetErrorQualifier(&qualifier, &szErrorQual)))
	{
		PrintMessage(ML_ERROR, L"Extra info: 0x%08X - %s\n", qualifier, szErrorQual ? CUTF8ToWChar(szErrorQual).c_str() : L"Unknown");
	}

	va_end(args);
}

void TargetCommand::PrintMessage(MSG_LEVEL nMsgLevel, const WCHAR *pszFormat, ...)
{
	if (!ms_TargetCommandObj)
		return;

	if (nMsgLevel == ML_INFO && ms_TargetCommandObj->SurpressErrorLogging())
		return;

	std::wstring Message;
	if (nMsgLevel == ML_WARN)
		Message = L"Warning - ";
	else if (nMsgLevel == ML_INFO)
		Message = L"Info - ";
	else
		Message = L"Error - ";

	va_list	args;
	va_start(args, pszFormat);

	WCHAR szBuffer[4 * 1024];

	_vsnwprintf(szBuffer, _countof(szBuffer), pszFormat, args);

	va_end(args);

	if (!ms_TargetCommandObj->GetName().empty())
	{
		Message += L"[";
		Message += UTF8ToWChar(ms_TargetCommandObj->GetName());
		Message += L"] ";
	}

	Message += szBuffer;

	_putws(Message.c_str());

	if (ms_TargetCommandObj->LoggingToFileEnabled())
	{
		FILE* f = _wfopen(UTF8ToWChar(ms_TargetCommandObj->GetLoggingPath()).c_str(), L"a");
		if (f)
		{
			fwprintf(f, Message.c_str());
			fclose(f);
		}
	}

}

int TargetCommand::Run()
{
	// Init the comms
	if (!Init())
		return GetErrorCodeOnError();

	// Check for add targets
	if (m_addTargetsVect.size() > 0)
	{
		if (!AddTargets(m_targetId))
			return GetErrorCodeOnError();

		if (m_targetName.empty())
		{
			// We didn't pass a target with -t so explicitly set the target name to the first in the list
			m_targetName = m_addTargetsVect[0].TargetName;
		}
	}

	// Check for delete targets
	if (m_deleteTargetsNames.size() > 0)
	{
		if (!DeleteTargets())
			return GetErrorCodeOnError();
	}

	if (!m_defaultTarget.empty())
	{
		if (!SetDefaultTarget())
			return GetErrorCodeOnError();
	}

	// Do we need to connect to target
	bool bNeedToConnectToTarget = !m_homeDirectoryPath.empty() || !m_fileServingPath.empty(); // Force connect if we have to set home and file serving paths
	if (m_bConnectToTarget || bNeedToConnectToTarget)
	{
		// Attempt to connect to target
		if (!SetUpTarget() || !ConnectToActiveTarget())
		{
			PrintMessage(ML_ERROR, L"Error connecting to target %s!\n", UTF8ToWChar(m_targetName).c_str());
			return GetErrorCodeOnError();
		}
	}

	if (!m_homeDirectoryPath.empty())
		if (!SetHomeDir())
			return GetErrorCodeOnError();
	if (!m_fileServingPath.empty())
		if (!SetFSDir())
			return GetErrorCodeOnError();

	if (SN_FAILED(SNPS3EnableInternalKick(TRUE)))
	{
		PrintMessage(ML_WARN, L"Could not set internal kick flag");
	}

	return m_exitCode;
}

bool TargetCommand::GetTargetFromAddress(const char *pszIPAddr, HTARGET &hTarget)
{
	TMAPI_TCPIP_CONNECT_PROP oConnection;
	std::vector<SNPS3TargetInfo*>::iterator iter = ms_Targets.begin();

	while (iter != ms_Targets.end())
	{
		if (SN_SUCCEEDED( SNPS3GetConnectionInfo((*iter)->hTarget, &oConnection) ))
		{
			if (wcscmp(UTF8ToWChar(std::string(pszIPAddr)).c_str(), CUTF8ToWChar(oConnection.szIPAddress)) == 0)
			{
				hTarget = (*iter)->hTarget;
				return true;
			}
		}

		++iter;
	}

	// If we didn't find a match there, do a DNS lookup
	std::string ipAddress;
	std::string dnsName;
	if (!GetHostnames(pszIPAddr, ipAddress, dnsName))
		return false;

	// Now iterate again
	iter = ms_Targets.begin();

	while (iter != ms_Targets.end())
	{
		if (SN_SUCCEEDED( SNPS3GetConnectionInfo((*iter)->hTarget, &oConnection) ))
		{
			if (wcscmp(UTF8ToWChar(ipAddress).c_str(), CUTF8ToWChar(oConnection.szIPAddress)) == 0
				|| wcscmp(UTF8ToWChar(dnsName).c_str(), CUTF8ToWChar(oConnection.szIPAddress)) == 0)
			{
				hTarget = (*iter)->hTarget;
				return true;
			}
		}

		++iter;
	}

	return false;
}

bool TargetCommand::GetHostnames(const char* input, std::string& ipOut, std::string& dnsNameOut)
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return false;
	}

	sockaddr_in remotemachine;
	char hostname[NI_MAXHOST];

	remotemachine.sin_family = AF_INET;
	remotemachine.sin_addr.s_addr = inet_addr(input);

	// IP->Hostname
	DWORD dwRetVal = getnameinfo((SOCKADDR *)&remotemachine, 
		sizeof(sockaddr), 
		hostname, 
		NI_MAXHOST, 
		NULL, 
		0, 
		NI_NAMEREQD);

	if (dwRetVal == 0)
	{
		dnsNameOut = hostname;
		return true;
	}

	// Hostname -> IP
	struct hostent *remoteHost;
	remoteHost = gethostbyname(input);

	int i = 0;
	struct in_addr addr = { 0 };
	if (remoteHost && remoteHost->h_addrtype == AF_INET)
	{
		if (remoteHost->h_addr_list[0] != 0)
		{
			addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
			ipOut = inet_ntoa(addr);
			return true;
		}
	}

	WSACleanup();
	return false;
}

int __stdcall TargetCommand::EnumCallBack(HTARGET hTarget)
{
	SNPS3TargetInfo ti;
	std::auto_ptr<SNPS3TargetInfo> pti(new SNPS3TargetInfo);

	if (pti.get())
	{
		ti.hTarget = hTarget;
		ti.nFlags = SN_TI_TARGETID;

		if (SN_S_OK == SNPS3GetTargetInfo(&ti))
		{
			// Store target parameters.
			pti->hTarget = hTarget;
			pti->pszName = _strdup(ti.pszName);
			pti->pszHomeDir = _strdup(ti.pszHomeDir);
			pti->pszFSDir = _strdup(ti.pszFSDir);

			// Store this target.
			ms_Targets.push_back(pti.release());
		}
		else
		{
			// Terminate enumeration.
			return 1;
		}
	}

	// Carry on with enumeration.
	return 0;
}

bool TargetCommand::FindFirstConnectedTarget(void)
{
	ECONNECTSTATUS nStatus = (ECONNECTSTATUS) -1;
	char*  pszUsage = 0;

	std::vector<SNPS3TargetInfo*>::iterator iter = ms_Targets.begin();

	while (iter != ms_Targets.end())
	{
		SNRESULT snr = SNPS3GetConnectStatus((*iter)->hTarget,	&nStatus, &pszUsage);

		if (SN_SUCCEEDED( snr ))
		{
			if (nStatus == CS_CONNECTED)
			{
				SNPS3TargetInfo ti;

				ti.hTarget = (*iter)->hTarget;
				ti.nFlags = SN_TI_TARGETID;

				if (SN_S_OK == SNPS3GetTargetInfo(&ti))
				{
					// Store target parameters.
					ms_TargetCommandObj->SetTargetId(ti.hTarget);
					ms_TargetCommandObj->SetTargetName(ti.pszName);

					return true;
				}
			}
		}
		iter++;
	}

	return false;
}

bool TargetCommand::FindFirstAvailableTarget(void)
{
	UINT   nStatus = -1;
	char*  pszUsage = 0;

	std::vector<SNPS3TargetInfo*>::iterator iter = ms_Targets.begin();

	while (iter != ms_Targets.end())
	{
		SNRESULT snr = SNPS3Connect((*iter)->hTarget, NULL);

		if (SN_SUCCEEDED( snr ))
		{
			SNPS3TargetInfo ti;

			ti.hTarget = (*iter)->hTarget;
			ti.nFlags = SN_TI_TARGETID;

			if (SN_S_OK == SNPS3GetTargetInfo(&ti))
			{
				// Store target parameters.
				ms_TargetCommandObj->SetTargetId(ti.hTarget);
				ms_TargetCommandObj->SetTargetName(ti.pszName);
				return true;
			}
		}

		iter++;
	}

	return false;
}

bool TargetCommand::Init()
{
	SNRESULT snr = InitTargetComms();
	if (SN_FAILED(snr))
		return false;

	return true;
}

bool TargetCommand::SetUpTarget()
{
	SNRESULT snr;

	// Enumerate available targets.
	if (SN_FAILED( snr = SNPS3EnumerateTargets(EnumCallBack) ))
	{
		PrintError(snr, L"Failed to enumerate targets");
		return false;
	}

	// Attempt to get the target name from an environment variable...

	if (m_bLaunchTargetPicker)
	{
		PrintMessage(ML_INFO, L"Launching target picker...");
		if (SN_FAILED(snr = SNPS3PickTarget(NULL, &m_targetId)))
		{
			PrintError(snr, L"Failed to pick target");
			return false;
		}

		SNPS3TargetInfo targetInfo = {};
		targetInfo.hTarget = m_targetId;
		targetInfo.nFlags = SN_TI_TARGETID;

		if (SN_FAILED( snr = SNPS3GetTargetInfo(&targetInfo) ))
		{
			PrintError(snr, L"Failed to get target info");
			return false;
		}

		m_targetName = std::string(targetInfo.pszName);
	}

	if (m_targetName.empty())
	{
		wchar_t* pEnv = _wgetenv(L"PS3TARGET");
		if (pEnv)
			m_targetName = WCharToUTF8(pEnv);
	}

	if (ms_Targets.size() == 1 && m_targetName.empty())
	{
		m_targetName = ms_Targets[0]->pszName;
	}

	// If no target has been selected then use the default target
	if (m_targetName.empty())
	{
		if (SN_S_OK == SNPS3GetDefaultTarget(&m_targetId))
		{
			SNPS3TargetInfo targetInfo = {};
			targetInfo.hTarget = m_targetId;
			targetInfo.nFlags = SN_TI_TARGETID;

			if (SN_FAILED( snr = SNPS3GetTargetInfo(&targetInfo) ))
			{
				PrintError(snr, L"Failed to get target info");
				return false;
			}

			m_targetName = std::string(targetInfo.pszName);
		}
	}
	
	// If no target has been selected then use the first one connected or the first one available.
	if (m_targetName.empty())
	{
		if (!FindFirstConnectedTarget())
		{
			FindFirstAvailableTarget();
		}
	}
	// Retrieve the target ID from the name or failing that IP.
	if (SN_FAILED(snr = SNPS3GetTargetFromName(m_targetName.c_str(), &m_targetId)))
	{
		if (!GetTargetFromAddress(m_targetName.c_str(), m_targetId))
		{
			PrintError(snr, L"Failed to find target! Please ensure target name/ip/hostname is correct");
			return false;
		}
	}

	return true;
}

bool TargetCommand::AddTargets(HTARGET &firstTargetId)
{
	_ASSERT(m_addTargetsVect.size() > 0);
	if (m_addTargetsVect.size() == 0)
		return false;

	std::vector<TARGET_ADD_DATA>::iterator it = m_addTargetsVect.begin();

	bool bFirstAdded = true;
	for(; it != m_addTargetsVect.end(); ++it)
	{
		TARGET_ADD_DATA &data = *it;

		TMAPI_TCPIP_CONNECT_PROP connProperties;
		memset(&connProperties, 0x00, sizeof(connProperties));
		UINT32 connSize = 0;

		if (!data.Hostname.empty())
		{
			strcpy_s(connProperties.szIPAddress, _countof(connProperties.szIPAddress)-1, data.Hostname.c_str());
			connProperties.uPort = data.Port;
			connSize = sizeof(connProperties);
		}

		HTARGET targetId;

		SNRESULT res = SNPS3AddTarget(data.TargetName.c_str(), data.TargetType.c_str(), connSize, (BYTE*)&connProperties, &targetId);
		if (SN_FAILED(res))
		{
			PrintError(res, L"Failed to add target");
			return false;
		}

		if (bFirstAdded)
			firstTargetId = targetId;
		bFirstAdded = false;

		PrintMessage(ML_INFO, L"Target <%s> successfully created: id=%d\n", CUTF8ToWChar(data.TargetName).c_str(), targetId);
	}

	return true;
}

bool TargetCommand::DeleteTargets()
{
	if (m_deleteTargetsNames.size() == 0)
		return false;

	StringIterator it = m_deleteTargetsNames.begin();
	bool bSuccess = false;
	for (; it != m_deleteTargetsNames.end(); ++it)
	{
		std::string &targetName = *it;
		HTARGET targetId;
		SNRESULT err = SNPS3GetTargetFromName(targetName.c_str(), &targetId);
		if (SN_FAILED(err))
		{
			PrintError(err, L"Failed to delete target <%s>", CUTF8ToWChar(targetName).c_str());
			continue;
		}

		err = SNPS3DeleteTarget(targetId);
		if (SN_FAILED(err))
		{
			PrintError(err, L"Failed to delete target <%s>", CUTF8ToWChar(targetName).c_str());
			continue;
		}
		PrintMessage(ML_INFO, L"Target <%s> successfully deleted", CUTF8ToWChar(targetName).c_str());
		bSuccess = true;
	}

	return bSuccess;
}

bool TargetCommand::SetDefaultTarget()
{
	if (m_defaultTarget.empty())
	{
		_ASSERT(false);
		return false;
	}

	HTARGET targetId;
	SNRESULT err = SNPS3GetTargetFromName(m_defaultTarget.c_str(), &targetId);
	if (SN_FAILED(err))
	{
		PrintError(err, L"Failed to find target <%s> when setting to default", CUTF8ToWChar(m_defaultTarget).c_str());
		return false;
	}

	err = SNPS3SetDefaultTarget(targetId);

	if (SN_FAILED(err))
	{
		PrintError(err, L"Failed set default target <%s>", CUTF8ToWChar(m_defaultTarget).c_str());
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully set default target <%s>\n", CUTF8ToWChar(m_defaultTarget).c_str());

	return true;
}

bool TargetCommand::ConnectToActiveTarget()
{
	char* pszUsage = NULL;
	SNRESULT snr;
	// Connect to the target.
	if (SN_FAILED(snr = SNPS3Connect(m_targetId, NULL)))
	{
		if (snr == SN_E_TARGET_IN_USE && m_bForceDC)
		{
			if (SN_FAILED( snr = SNPS3ForceDisconnect(m_targetId) ))
			{
				PrintError(snr, L"Unable to force disconnect %s", CUTF8ToWChar(pszUsage).c_str());
				return false;
			}
			else
			{
				snr = SNPS3Connect(m_targetId, NULL);
			}
		}

		if (SN_FAILED(snr))
		{
			PrintError(snr, L"Failed to connect to target");
			return false;
		}
	}
	else
	{
		m_bWasOriginallyConnected = (snr == SN_S_NO_ACTION);
	}

	PrintMessage(ML_INFO, L"Connected to target\n");
	return true;
}

bool TargetCommand::SetFSDir()
{
	if (m_fileServingPath.empty())
		return false;

	SNRESULT snr = SN_S_OK;

	SNPS3TargetInfo ti;

	WCHAR  szBuffer[MAX_PATH];
	WCHAR* pszFilePart = 0;

	DWORD nRes = GetFullPathName(UTF8ToWChar(m_fileServingPath).c_str(), MAX_PATH, szBuffer, &pszFilePart);

	if (nRes == 0 || nRes > MAX_PATH)
	{
		PrintMessage(ML_ERROR, L"Illegal file serving directory \"%s\" specified\n",		
			m_fileServingPath);

		return false;
	}
	std::string strFSDir;
	WCharToUTF8(szBuffer, strFSDir);
	ti.hTarget  = m_targetId;
	ti.nFlags   = SN_TI_FILESERVEDIR | SN_TI_TARGETID;
	ti.pszFSDir = strFSDir.c_str();

	if (SN_FAILED(snr = SNPS3SetTargetInfo(&ti)))
	{
		PrintError(snr, L"Failed to set file serving directory");
		return false;
	}

	PrintMessage(ML_INFO, L"File serving directory changed to \"%s\"\n", szBuffer);

	return true;
}

bool TargetCommand::SetHomeDir()
{
	if (m_homeDirectoryPath.empty())
		return false;

	SNRESULT snr = SN_S_OK;

	SNPS3TargetInfo ti;

	WCHAR  szBuffer[MAX_PATH];
	WCHAR* pszFilePart = 0;

	DWORD nRes = GetFullPathName(UTF8ToWChar(m_homeDirectoryPath).c_str(), MAX_PATH, szBuffer, &pszFilePart);

	if (nRes == 0 || nRes > MAX_PATH)
	{
		PrintMessage(ML_ERROR, L"Illegal home directory \"%s\" specified\n",
			m_homeDirectoryPath);

		return false;
	}

	ti.hTarget    = m_targetId;
	ti.nFlags     = SN_TI_HOMEDIR | SN_TI_TARGETID;
	std::string strHomeDir;
	WCharToUTF8(szBuffer, strHomeDir);
	ti.pszHomeDir = strHomeDir.c_str();

	if (SN_FAILED( snr = SNPS3SetTargetInfo(&ti) ))
	{
		PrintError(snr, L"Failed to set home directory");
		return false;
	}

	PrintMessage(ML_INFO, L"Home directory changed to \"%s\"\n",	szBuffer);
	return true;
}

bool TargetCommand::ExtractTargetDetails(std::string& str, TARGET_ADD_DATA& data)
{
	// The string is in the following format:
	// "TargetName:TargetType:Hostname:Port"
	// All are optional

	_ASSERT(!str.empty());
	if (str.empty())
		return false;

	std::istringstream strStream(str);

	std::string tokenVal;

	for (int i = 0; i < 4 && getline(strStream, tokenVal, ':'); ++i)
	{
		switch(i)
		{
		case 0:
			data.TargetName = tokenVal;
			break;
		case 1:

			data.TargetType = GetTargetType(tokenVal);
			if (data.TargetType.empty())
				return false;
			break;
		case 2:
			data.Hostname = tokenVal;
			break;
		case 3:
			if (!ReadInt(tokenVal, data.Port))
				return false;
			break;
		default:
			_ASSERT(false);
			return false;
		}
	}

	return true;
}

std::string TargetCommand::GetTargetType(std::string& str)
{
	if (str == "PS3_DEH_TCP" || str == "ref1000")
		return "PS3_DEH_TCP";
	if (str == "PS3_512_DBG_DEX" || str == "ref1400")
		return "PS3_512_DBG_DEX";
	if (str == "PS3_DBG_DEX" || str == "debug")
		return "PS3_DBG_DEX";
	if (str == "PS3_CORE_DUMP" || str == "coredump")
		return "PS3_CORE_DUMP";

	PrintMessage(ML_WARN, L"Invalid target type specified: %s. Using default PS3_DEH_TCP instead\n", UTF8ToWChar(str).c_str());

	return ""; // default
}

void TargetCommand::SetExitCode(UINT32 exitCode)
{
	m_exitCode = exitCode;
}

void TargetCommand::DisplayCommonOptions() const
{
	std::cout << "  -t <target>" << "\t" << "Specify target (defaults to default target)" << std::endl;
	std::cout << "  -pt" << "\t" << "Choose target using target picker dialog" << std::endl;
	std::cout << "  -add" << "\t\t" << "Add targets <t1> ..<tn> Format: \"TargetName:type:host:port\"" << std::endl;
	std::cout << "  -del" << "\t\t" << "Delete targets <t1> ..<tn>" << std::endl;
	std::cout << "  -sd <target>" << "\t" << "Set the default target" << std::endl;
	std::cout << "  -d" << "\t\t" << "Disconnect from target (if not previously connected)" << std::endl;
	std::cout << "  -da" << "\t\t" << "Always disconnect from target when done" << std::endl;
	std::cout << "  -na" << "\t\t" << "No disconnect (default)" << std::endl;
	std::cout << "  -fd" << "\t\t" << "Force disconnect if another user is connected" << std::endl;
	std::cout << "  -nc" << "\t\t" << "Do not connect" << std::endl;
	std::cout << "  -q" << "\t\t" << "Operate quietly if no errors" << std::endl;
	std::cout << "  -f <path>" << "\t" << "Set file serving directory" << std::endl;
	std::cout << "  -h <path>" << "\t" << "Set home directory" << std::endl;
	std::cout << "  -l <path>" << "\t" << "Log output to a file" << std::endl;
}

void TargetCommand::DisplayUsageVersionAndCopyright() const
{
	std::cout << "PS3Ctrl v" STRINGIFY(TM_VERSION_DOTS) " ("__DATE__") - PS3TM Command Line Tool" << std::endl;
	std::cout << STR_PRODG_COPYRIGHT << std::endl;
}

void TargetCommand::DisplayUsageHelp() const
{
	std::cout << "  Usage: PS3Ctrl [command] <options> [<args>]" << std::endl << std::endl;
	std::cout << "  Where <command> can be one of the following: " << std::endl << std::endl;

	for (size_t i = 0; i < g_Commands.size() ; i++)
	{
		std::cout << "     " << g_Commands[i].GetShortName() <<  std::endl;
	}
	std::cout << std::endl;
	std::cout << "  and where <options> can be any of the following" << std::endl;
	DisplayCommonOptions();
	std::cout << std::endl;
	std::cout << "Type 'PS3Ctrl [command] help' for information on command usage" << std::endl;
}

void TargetCommand::ShowUsage() const
{
	DisplayUsageVersionAndCopyright();
	std::cout << std::endl;

	DisplayUsageHelp();
}
