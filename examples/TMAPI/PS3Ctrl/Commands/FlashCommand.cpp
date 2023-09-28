/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "FlashCommand.h"

TargetCommand* FlashCommandFactory(void)
{
	return new FlashCommand();
}

FlashCommand::FlashCommand()
: TargetCommand()
{

}

FlashCommand::~FlashCommand()
{

}

bool FlashCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<std::string> up("up", "tool-updater-path", PS3FLASH_DEFAULT_UPDATER);
	SingleArgOption<std::string> pup("pup", "pup-path", PS3FLASH_DEFAULT_PUP);

	m_cmdLineHandler.AddArgument(up);
	m_cmdLineHandler.AddArgument(pup);

	m_cmdLineHandler.Parse(arguments);

	if (up.IsPassed())
		m_toolUpdaterPath = up.GetValue();

	m_pupPath = pup.GetValue();

	return true;
}

int FlashCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	const char *pToolUpdater = NULL;

	if (!m_toolUpdaterPath.empty())
		pToolUpdater  = m_toolUpdaterPath.c_str();
	else
	{
		PrintMessage(ML_INFO, L"No updater path specified - using built in updater tool");
	}

	const char *pPUPPath = m_pupPath.c_str();

	SNRESULT snr;
	if (SN_FAILED( snr = SNPS3FlashTarget(m_targetId, pToolUpdater, pPUPPath)))
	{
		PrintError(snr, L"Error - Failed to load updater tool");
		return snr;
	}

	PrintMessage(ML_INFO, L"Updated flash successfully!");

	return m_exitCode;
}

void FlashCommand::DisplayUsageHelp() const
{
	std::cout << "The flash command allows you to flash a target." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl flash <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -up <path>" << "\t" << "Location of PS3ToolUpdater.self" << std::endl;
	std::cout << "  -pup <path>" << "\t" << "Location of PS3UPDAT.PUP" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
