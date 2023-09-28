/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "InstallGameCommand.h"

TargetCommand* InstallGameCommandFactory(void)
{
	return new InstallGameCommand();
}

InstallGameCommand::InstallGameCommand()
: TargetCommand()
, m_bWaitForTransfer(false)
, m_waitTimeout(0)
{

}

InstallGameCommand::~InstallGameCommand()
{

}

bool InstallGameCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<UINT32> wf("wf", "wait-for-transfer", 0, false, false);

	m_cmdLineHandler.AddArgument(wf);

	m_cmdLineHandler.Parse(arguments);

	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
	if (remainingArgs.size() == 0)
		throw ArgumentException("You need to specify the SFO path of the game!");
	else
		m_sfoPath = remainingArgs[0];

	if (wf.IsPassed())
	{
		m_bWaitForTransfer = true;
		m_waitTimeout = wf.GetValue();
	}

	return true;
}

int InstallGameCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (!DoInstallGame())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool InstallGameCommand::DoInstallGame()
{
	_ASSERT(!m_sfoPath.empty());
	if (m_sfoPath.empty())
		return false;

	char* pszTitle;
	char* pszTargetPath;
	UINT32 transferId = 0;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3InstallGameEx(m_targetId, m_sfoPath.c_str(), &pszTitle, &pszTargetPath, &transferId)))
	{
		PrintError(snr, L"Error installing game. SFO path supplied:%s\n", UTF8ToWChar(m_sfoPath).c_str());
		return false;
	}

	TMAPI_FT_NOTIFY ftValue;

	if (m_bWaitForTransfer)
	{
		PrintMessage(ML_INFO, L"Waiting for game to install with timeout = %dms...\n", m_waitTimeout);
		if (SN_SUCCEEDED(SNPS3WaitForFileTransfer(m_targetId, transferId, &ftValue, m_waitTimeout)))
		{
			std::string notifyValue = GetFTText(ftValue);
			PrintMessage(ML_INFO, L"Wait complete. Notify Value: %s\n", UTF8ToWChar(notifyValue).c_str());
		}
		else
		{
			PrintMessage(ML_ERROR, L"Timed out when waiting for game to install!\n");
			return true;
		}
	}

	PrintMessage(ML_INFO, L"Successfully installed game. Title: %s | Target Path: %s", UTF8ToWChar(pszTitle).c_str(), UTF8ToWChar(pszTargetPath).c_str());

	return true;
}

std::string InstallGameCommand::GetFTText(TMAPI_FT_NOTIFY& val)
{
	std::string strVal;

	switch (val)
	{
	case TMAPI_FT_PROGRESS:
		strVal = "TMAPI_FT_PROGRESS";
		break;
	case TMAPI_FT_FINISH:
		strVal = "TMAPI_FT_FINISH";
		break;
	case TMAPI_FT_SKIPPED:
		strVal = "TMAPI_FT_SKIPPED";
		break;
	case TMAPI_FT_CANCELLED:
		strVal = "TMAPI_FT_CANCELLED";
		break;
	case TMAPI_FT_ERROR:
		strVal = "TMAPI_FT_ERROR";
		break;
	case TMAPI_FT_UNKNOWN:
		strVal = "TMAPI_FT_UNKNOWN";
		break;
	}

	return strVal;
}

void InstallGameCommand::DisplayUsageHelp() const
{
	std::cout << "The install-game command allows you to install a game." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl install-game <options> <file>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -wf <timeout>" << "\t" << "Waits for install game to finish before exiting. Timeout in (ms)" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
