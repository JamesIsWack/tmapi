/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "SettingsCommand.h"

TargetCommand* SettingsCommandFactory(void)
{
	return new SettingsCommand();
}

SettingsCommand::SettingsCommand()
: TargetCommand()
	,m_Mode(MODE_NONE)
{
}

SettingsCommand::~SettingsCommand()
{

}

bool SettingsCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<std::string> imp("i", "import", "");
	SingleArgOption<std::string> exp("e", "export", "");

	m_cmdLineHandler.AddArgument(imp);
	m_cmdLineHandler.AddArgument(exp);

	m_cmdLineHandler.Parse(arguments);

	if (imp.IsPassed())
	{
		m_strPath = imp.GetValue();
		m_Mode = MODE_IMPORT;
	}
	else if (exp.IsPassed())
	{
		m_strPath = exp.GetValue();
		m_Mode = MODE_EXPORT;
	}
	else
	{
		m_Mode = MODE_NONE;
	}

	return true;
}

int SettingsCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	switch (m_Mode)
	{
	case MODE_IMPORT:
		if (!DoImport())
			return GetErrorCodeOnError();
		break;

	case MODE_EXPORT:
		if (!DoExport())
			return GetErrorCodeOnError();
		break;
	default:
		DisplayUsageHelp();
		return PS3CTRL_EXIT_ERROR;
	}

	return m_exitCode;
}

bool SettingsCommand::DoImport()
{
	_ASSERT(!m_strPath.empty());

	if (m_strPath.empty())
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3ImportTargetSettings(m_targetId, m_strPath.c_str())))
	{
		PrintError(snr, L"Error importing settings file. Path supplied:%s\n", UTF8ToWChar(m_strPath).c_str());
		return false;
	}

	PrintMessage(ML_INFO, L"Settings imported from %s", UTF8ToWChar(m_strPath).c_str());

	return true;
}

bool SettingsCommand::DoExport()
{
	_ASSERT(!m_strPath.empty());

	if (m_strPath.empty())
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3ExportTargetSettings(m_targetId, m_strPath.c_str())))
	{
		PrintError(snr, L"Error importing settings file. Path supplied:%s\n", UTF8ToWChar(m_strPath).c_str());
		return false;
	}

	PrintMessage(ML_INFO, L"Settings imported from %s", UTF8ToWChar(m_strPath).c_str());

	return true;
}

void SettingsCommand::DisplayUsageHelp() const
{
	std::cout << "The install-game command allows you to install a game." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl settings <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -i <filename>" << "\t" << "Imports the target settings from the file" << std::endl;
	std::cout << "  -e <filename>" << "\t" << "Exports the target settings to the file" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
