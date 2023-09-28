/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "FormatCommand.h"

TargetCommand* FormatCommandFactory(void)
{
	return new FormatCommand();
}

FormatCommand::FormatCommand()
: TargetCommand()
, m_bClearReg(false)
{

}

FormatCommand::~FormatCommand()
{

}

bool FormatCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	StandardOption cr("cr", "clear-registry");

	m_cmdLineHandler.AddArgument(cr);

	m_cmdLineHandler.Parse(arguments);

	if (cr.IsSet())
		m_bClearReg = true;

	return true;
}

int FormatCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (!DoFormatHDD())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool FormatCommand::DoFormatHDD()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3FormatHDD(m_targetId, m_bClearReg ? 1 : 0)))
	{
		PrintError(snr, L"Error formating hard drive!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully started hard drive format!\n");
	return true;
}

void FormatCommand::DisplayUsageHelp() const
{
	std::cout << "The format command allows you to format a target's hard drive." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl format <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -cr" << "\t\t" << "Clear the registry" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
