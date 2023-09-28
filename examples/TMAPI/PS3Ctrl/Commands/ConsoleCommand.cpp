/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "ConsoleCommand.h"

TargetCommand* ConsoleCommandFactory(void)
{
	return new ConsoleCommand();
}

ConsoleCommand::ConsoleCommand()
: TargetCommand()
, m_bConsoleInChannel(-1) // Invalid stdin channel
, m_bConsoleIn(false)
, m_bConsoleOut(false)
, m_bCheckForAbort(false)
, m_bRelTimeoutSet(false)
, m_relTimeoutTime(0)
, m_bAbsTimeoutSet(false)
, m_absTimeoutTime(0)
, m_relTimeoutDuration(0)
, m_absTimeoutDuration(0)
, m_bReadStdInFromFile(false)
, m_pConsoleImportSource(NULL)
, m_abortExitCode(PS3CTRL_EXIT_OK)
{

}

ConsoleCommand::~ConsoleCommand()
{

}

bool ConsoleCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<std::string> c("c", "channel", "");
	SingleArgOption<std::string> i("i", "input-file", "");
	
	i.SetParentDependency(&c);

	MultiArgOption<std::string> p("p","print-channel");
	p.SetValidValues(GetTTYStreams());
	SingleArgOption<std::string> a("a", "abort-console-text", "");
	SingleArgOption<std::string> e("e", "exit-console-text", "");

	SingleArgOption<UINT64> y("y", "timeout", 0);
	SingleArgOption<UINT64> z("z", "timeout-no-event", 0);

	a.SetImpliedArgument(&p);
	e.SetImpliedArgument(&p);

	m_cmdLineHandler.AddArgument(c);
	m_cmdLineHandler.AddArgument(i);
	m_cmdLineHandler.AddArgument(p);
	m_cmdLineHandler.AddArgument(a);
	m_cmdLineHandler.AddArgument(e);
	m_cmdLineHandler.AddArgument(z);
	m_cmdLineHandler.AddArgument(y);

	m_cmdLineHandler.Parse(arguments);

	if (c.IsPassed())
	{
		m_bConsoleIn = true;
		m_stdinChannel = c.GetValue();

		if (!i.GetValue().empty())
		{
			m_bReadStdInFromFile = true;
			std::string& filePath = i.GetValue();

			if (filePath == "stdin")
				m_pConsoleImportSource = stdin;
			else
			{
				// It's a file
				WCHAR *pchFilePart = NULL;
				WCHAR szFile[MAX_PATH] = {'\0'};

				DWORD dwRes = GetFullPathNameW(
					UTF8ToWChar(filePath).c_str(), 
					_countof(szFile), 
					szFile, 
					&pchFilePart);

				if (dwRes == 0 || dwRes > _countof(szFile))
					throw ArgumentException("Error - Illegal file:  specified");

				m_pConsoleImportSource = _wfopen(szFile, L"r");

				if (m_pConsoleImportSource == NULL)
					throw ArgumentException("Error - Illegal file:  specified");
			}
		}
	}

	if (p.IsPassed())
	{
		m_bConsoleOut = true;
		if (p.GetValues().size() == 0)
			m_stdoutChannels.push_back("all");
		else
		{
			std::vector<std::string> &channelValuesVect = p.GetValues();
			std::vector<std::string>::const_iterator it = channelValuesVect.begin();
			for (; it != channelValuesVect.end(); ++it)
				m_stdoutChannels.push_back(*it);
		}

		if (a.IsPassed())
		{
			m_bCheckForAbort = true;
			m_abortString = UTF8ToWChar(a.GetValue());
			m_abortExitCode = PS3CTRL_EXIT_ERROR;
		} else if (e.IsPassed())
		{
			m_bCheckForAbort = true;
			m_abortString = UTF8ToWChar(e.GetValue());
			m_abortExitCode = PS3CTRL_EXIT_OK;
		}
	}

	if (z.IsPassed())
	{
		m_bRelTimeoutSet = true;
		m_relTimeoutDuration = z.GetValue();
	} 
	else if (y.IsPassed())
	{
		m_bAbsTimeoutSet = true;
		m_absTimeoutDuration = y.GetValue();

	}

	m_cmdLineHandler.Reset();

	return true;
}

int ConsoleCommand::Run()
{
	int bRes = TargetCommand::Run();

	if (SN_FAILED(bRes))
		return bRes;

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

	if (m_bAbsTimeoutSet)
	{
		_time64(&m_absTimeoutTime);
		m_absTimeoutTime += m_absTimeoutDuration;
	}

	ClearMessagesBeforeExit();
	if (ProcessEvents() == false)
		return GetErrorCodeOnError();

	if (DisconnectTTYEvents() == false)
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool ConsoleCommand::InitializeTTYStreams()
{
	SNRESULT snr = SN_S_OK;

	UINT32 numStreams;
	// Get number of available TTY channels.
	if (SN_FAILED( snr = SNPS3ListTTYStreams(m_targetId, &numStreams, NULL) ))
	{
		PrintError(snr, L"Failed to get count of TTY streams");
		return false;
	}

	TTYSTREAM *pTTYStreams = new TTYSTREAM[numStreams];

	// Get list of available TTY channels.
	if (SN_FAILED( snr = SNPS3ListTTYStreams(m_targetId, &numStreams, pTTYStreams) ))
	{
		PrintError(snr, L"Failed to list TTY streams");
		delete [] pTTYStreams;
		return false;
	}

	for (UINT32 i = 0; i < numStreams; ++i)
	{
		TTYSTREAM pStream = pTTYStreams[i];
		m_ttyStreams.push_back(pStream);
	}

	delete[] pTTYStreams;

	return true;
}

UINT32 ConsoleCommand::ResolveTTYStreamName(std::string& streamName)
{
	std::vector<TTYSTREAM>::const_iterator it = m_ttyStreams.begin();
	for (; it != m_ttyStreams.end(); ++it)
	{
		TTYSTREAM stream = *it;
		if (_stricmp(stream.szName, streamName.c_str()) == 0)
			return stream.nStreamIdx;
	}

	UINT32 streamId = -1;

	// Otherwise fallback to assuming it's an index.
	if (1 == sscanf(streamName.c_str(), "%u", &streamId))
	{
		std::vector<TTYSTREAM>::const_iterator it = m_ttyStreams.begin();
		for (; it != m_ttyStreams.end(); ++it)
		{
			TTYSTREAM stream = *it;
			if (stream.nStreamIdx == streamId)
				return streamId;
		}
	}

	return -1;
}

void __stdcall ConsoleCommand::ProcessTTYCallback(HTARGET /*hTarget*/, UINT uiType, UINT uiStreamId, 
										 SNRESULT /*Result*/, UINT uiLength, BYTE *pData, void* /*pUser*/)
{
	if (uiType == SN_EVENT_TTY)
	{
		char* szData = (char*) _alloca(uiLength+1);
		strncpy(szData, (char*) pData, uiLength);
		szData[uiLength] = '\0';

		char* pCtrlZ = strchr(szData, 26);

		if (pCtrlZ)
			*pCtrlZ = 0;

		// Output the received TTY text.
		printf("%s", szData);

		// Test for abort string.
		ConsoleCommand *pCommandObj = static_cast<ConsoleCommand*>(ms_TargetCommandObj);

		if (pCommandObj && pCommandObj->CheckForAbort() && !pCommandObj->AbortKick() && pCommandObj->FindAbort(CUTF8ToWChar(szData)))
		{
			pCommandObj->AbortOnTextMatch();
			PrintMessage(ML_INFO, L"Found abort text. PS3Ctrl will exit after the current commands are completed.\n");
		}

		// If Ctrl Z found, exit the message pump.
		if (pCtrlZ)
		{
			if (*(pCtrlZ - 1) != '\n')
				puts("");

			ms_TargetCommandObj->SetAbortKick();
		}

		fflush(stdout);
	}
}

bool ConsoleCommand::FindAbort(const wchar_t *pszText)
{
	m_consoleBuffer += pszText;

	// Search the TTY for the abort string
	if (wcsstr(m_consoleBuffer.c_str(), m_abortString.c_str()))
		return true;
	

	size_t nAbortStrLen = wcslen(m_abortString.c_str());

	if (m_consoleBuffer.length() > nAbortStrLen)
	{
		m_consoleBuffer.erase(0, m_consoleBuffer.length() - nAbortStrLen);
	}

	return false;
}

bool ConsoleCommand::ProcessEvents()
{
	// Initialize the the timeout time
	if (m_bRelTimeoutSet) 
	{
		_time64(&m_relTimeoutTime);
		m_relTimeoutTime += m_relTimeoutDuration;
	}	

	if (IS_VERBOSE)
	{
		if (m_bConsoleIn)
		{
			if (m_bReadStdInFromFile)
			{
				PrintMessage(ML_INFO, L"Sending input source to channel '%s'\n", UTF8ToWChar(m_stdinChannel).c_str());
			}
			else
			{
				PrintMessage(ML_INFO, L"Sending keystrokes to channel '%s'\n", UTF8ToWChar(m_stdinChannel).c_str());
			}
		}

		PrintMessage(ML_INFO, L"Listening for events, press ESC to quit...\n");
	}

	SNRESULT snr = ClearPendingMessages();

	while (SN_SUCCEEDED(snr) && ms_TargetCommandObj->AbortKick() == false)
	{
		if (m_bReadStdInFromFile)
		{
			if (GetChar() == ESCAPE_KEY)
				return true;

			bool bEndOfFile = false;
			std::string strLine = GetLine(bEndOfFile);
			const char *tty = strLine.c_str();

			if (bEndOfFile)
			{
				if (!SendTTY(tty))
				{
					PrintMessage(ML_ERROR, L"Failed to send TTY '%s'\n", CUTF8ToWChar(tty).c_str());
					return false;
				}

				// If we have read the file wait on events
				if (!WaitOnEvents())
					return true;
			} 
			else
			{
				// Send the current line read from the file
				if (!SendTTY(tty))
				{
					PrintMessage(ML_ERROR, L"Failed to send TTY '%s'\n", CUTF8ToWChar(tty).c_str());
					return false;
				}
			}
		} 
		else
		{
			char tty[] = { GetChar(), '\0'};

			switch (tty[0])
			{
			case '\0':
				// Nothing to send and we've cleared our messages 
				// so take a nap (prevents CPU hogging).
				CheckTimeout();
				::Sleep(40);
				break;

			case ESCAPE_KEY:
				return true;

			default:
				if (!SendTTY(tty))
				{
					PrintMessage(ML_ERROR, L"Failed to send TTY '%s'\n", CUTF8ToWChar(tty).c_str());
					return false;
				}
				// TODO: Do we need to sleep here to help prevent mangled output?
				break;
			}
		}
		
		snr = ClearPendingMessages();
	}

	return true;
}

SNRESULT ConsoleCommand::ClearPendingMessages()
{
	SNRESULT snr = SN_S_OK;
	int nKicks = 0;

	do
	{
		snr = SNPS3Kick();
		++nKicks;

	} while (snr == SN_S_OK && !m_bAbortKick);

	if (nKicks > 1)
		UpdateTimeout();

	return snr;
}

void ConsoleCommand::UpdateTimeout()
{
	if (m_bRelTimeoutSet)
	{
		__time64_t uNow;
		_time64(&uNow);

		m_relTimeoutTime = uNow + m_relTimeoutDuration;
	}
}

bool ConsoleCommand::ProcessRequestTTY()
{
	if (InitializeTTYStreams() == false)
		return false;

	StringIterator it = m_stdoutChannels.begin();
	for (; it != m_stdoutChannels.end(); ++it)
	{
		UINT32 channelId = ResolveTTYStreamName(*it);
		m_bConsoleOutChannels.push_back(channelId);
	}

	if (std::find(m_bConsoleOutChannels.begin(), m_bConsoleOutChannels.end(), -1) != m_bConsoleOutChannels.end())
	{
		m_bConsoleOutChannels.clear();
		m_bConsoleOutChannels.push_back(-1); // all channel
	}

	std::sort(m_bConsoleOutChannels.begin(), m_bConsoleOutChannels.end());
	m_bConsoleOutChannels.erase(
		std::unique(m_bConsoleOutChannels.begin(), m_bConsoleOutChannels.end()),
		m_bConsoleOutChannels.end());


	std::vector<UINT32>::const_iterator it2 = m_bConsoleOutChannels.begin();
	for (; it2 != m_bConsoleOutChannels.end(); ++it2)
	{
		UINT32 channelId = *it2;
		// Request TTY notification (-1 is all channels).
		SNRESULT snr;
		if (SN_FAILED( snr = SNPS3RegisterTTYEventHandler(m_targetId, channelId, ProcessTTYCallback, NULL) ))
		{
			PrintError(snr, L"Failed to register for TTY notifications on channel %d", channelId);
		}

		PrintMessage(ML_INFO, L"Registered for TTY output on %s channel\n", (channelId == -1) ? L"every" : CUTF8ToWChar(m_ttyStreams[channelId].szName));
	}

	return true;
}

void ConsoleCommand::ClearMessagesBeforeExit(void)
{
	Sleep(500);

	while (SNPS3Kick() == SN_S_OK)
		/* Do nothing */;
}

bool ConsoleCommand::DisconnectTTYEvents(void)
{
	// Cancel TTY notifications (-1 is all channels).
	for (size_t i = 0 ; i < m_bConsoleOutChannels.size() ; ++i)
	{
		SNRESULT snr;
		if (SN_FAILED( snr = SNPS3CancelTTYEvents(m_targetId, m_bConsoleOutChannels[i]) ))
		{
			PrintError(snr, L"Failed to cancel TTY notifications on %s channel", 
				(m_bConsoleOutChannels[i] == -1) ? L"every" : CUTF8ToWChar(m_ttyStreams[m_bConsoleOutChannels[i]].szName));
		}
	}

	return true;
}

char ConsoleCommand::GetChar()
{
	if (_kbhit())
		return _getch();

	return '\0';
}


std::string ConsoleCommand::GetLine(bool& bEof)
{
	std::string strLine;

	if (m_bConsoleIn && m_bReadStdInFromFile && m_pConsoleImportSource != NULL)
	{
		char line[MAX_LINE_SIZE];
		if (fgets (line , MAX_LINE_SIZE , m_pConsoleImportSource) != NULL)
		{
			strLine += line;
		} else if (feof(m_pConsoleImportSource))
		{
			strLine += EOF;
			fclose(m_pConsoleImportSource);
			m_pConsoleImportSource = NULL;
			bEof = true;
		}
	}

	return strLine;
}


void ConsoleCommand::CheckTimeout()
{
	if (m_bAbortKick)
		return;

	if (m_bAbsTimeoutSet || m_bRelTimeoutSet)
	{
		__time64_t uNow;
		_time64(&uNow);

		if ((m_bRelTimeoutSet) && uNow > m_relTimeoutTime)
		{
			m_bAbortKick = true;
			m_exitCode = PS3CTRL_EXIT_ERROR;
		}

		if ((m_bAbsTimeoutSet) && uNow > m_absTimeoutTime)
		{
			m_bAbortKick = true;
			m_exitCode = PS3CTRL_EXIT_ERROR;
		}

		if (m_bAbortKick)
		{
			PrintMessage(ML_WARN, L"Timeout occurred\n");
		}
	}
}

bool ConsoleCommand::SendTTY(const char* tty)
{
	if (m_bConsoleIn)
	{
		if (SN_FAILED( SNPS3SendTTY(m_targetId, m_bConsoleInChannel, tty)))
			return false;
	}

	return true;
}

bool ConsoleCommand::ProcessSendTTY(void)
{
	if (InitializeTTYStreams() == false)
		return false;

	m_bConsoleInChannel = ResolveTTYStreamName(m_stdinChannel);

	if (m_bConsoleInChannel == -1)
	{
		PrintMessage(ML_ERROR, L"Failed to resolve TTY stream '%s'\n", UTF8ToWChar(m_stdinChannel.c_str()).c_str());
		return false;
	}

	return true;
}

int ConsoleCommand::GetConsoleAbortExitCode()
{ 
	return m_abortExitCode; 
}

bool ConsoleCommand::CheckForAbort()
{ 
	return m_bCheckForAbort; 
}

void ConsoleCommand::AbortOnTextMatch()
{
	m_exitCode = m_abortExitCode;
	SetAbortKick();
}

bool ConsoleCommand::WaitOnEvents()
{
	return TargetCommand::WaitOnEvents() ||  m_bConsoleOut || m_bAbsTimeoutSet || m_bRelTimeoutSet;
}

void ConsoleCommand::DisplayUsageHelp() const
{
	std::cout << "The console command allows you to perform console related functions." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl console <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
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

std::vector<std::string> ConsoleCommand::GetTTYStreams()
{
	// The channels are hard coded here as we use this function when not connected to the target for initialisation purposes
	std::vector<std::string> channels;

	channels.push_back("ALL"); channels.push_back("all");
	channels.push_back("TM"); channels.push_back("tm");
	channels.push_back("KERNEL"); channels.push_back("kernel");
	channels.push_back("PPU"); channels.push_back("ppu");
	channels.push_back("PPU (STDERR)"); channels.push_back("ppu (stderr)");
	channels.push_back("SPU"); channels.push_back("spu");
	channels.push_back("USER 1"); channels.push_back("user 1");
	channels.push_back("USER 2"); channels.push_back("user 2");
	channels.push_back("USER 3"); channels.push_back("user 3");
	channels.push_back("USER 4"); channels.push_back("user 4");
	channels.push_back("USER 5"); channels.push_back("user 5");
	channels.push_back("USER 6"); channels.push_back("user 6");
	channels.push_back("USER 7"); channels.push_back("user 7");
	channels.push_back("USER 8"); channels.push_back("user 8");
	channels.push_back("USER 9"); channels.push_back("user 9");
	channels.push_back("USER 10"); channels.push_back("user 10");
	channels.push_back("USER 11"); channels.push_back("user 11");
	channels.push_back("USER 12"); channels.push_back("user 12");
	channels.push_back("USER 13"); channels.push_back("user 13");

	return channels;
}

