/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "SetDisplaySettingsCommand.h"

TargetCommand* SetDisplaySettingsCommandFactory(void)
{
	return new SetDisplaySettingsCommand();
}

SetDisplaySettingsCommand::SetDisplaySettingsCommand()
: TargetCommand(),
	m_dwMonitor(0),
	m_dwConnector(0),
	m_dwResolution(0),
	m_bHDCP(false),
	m_bReset(false),
	m_bArgsOk(false)
{

}

SetDisplaySettingsCommand::~SetDisplaySettingsCommand()
{

}

bool SetDisplaySettingsCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<std::string> monitor("monitor", "monitor","");
	SingleArgOption<std::string> connector("connector", "connector","");
	SingleArgOption<std::string> res("res", "resolution","");
	SingleArgOption<std::string> binary("bin", "binary","");
	StandardOption hdcp("hdcp", "hdcp");
	StandardOption reset("reset", "reset");

	m_cmdLineHandler.AddArgument(monitor);
	m_cmdLineHandler.AddArgument(connector);
	m_cmdLineHandler.AddArgument(res);
	m_cmdLineHandler.AddArgument(hdcp);
	m_cmdLineHandler.AddArgument(reset);
	m_cmdLineHandler.AddArgument(binary);
	m_cmdLineHandler.Parse(arguments);

	if (!monitor.IsPassed() ||
		!connector.IsPassed() ||
		!res.IsPassed())
	{
		DisplayUsageHelp();
		throw ArgumentException("Error - monitor, connector, res arguments are mandatory!");
	}
	if (binary.IsPassed())
	{
		m_strBinary = binary.GetValue();
	}

	m_dwMonitor = atoi(monitor.GetValue().c_str());
	m_dwConnector = atoi(connector.GetValue().c_str());
	m_dwResolution = atoi(res.GetValue().c_str());
	m_bHDCP = hdcp.IsPassed();
	m_bReset = reset.IsPassed();
	
	m_bArgsOk = true;

	m_cmdLineHandler.Reset();

	return true;
}

int SetDisplaySettingsCommand::Run()
{
	int bRes = TargetCommand::Run();

	if (SN_FAILED(bRes))
		return bRes;

	if (!m_bArgsOk)
	{
		DisplayUsageHelp();
		return PS3CTRL_EXIT_ERROR;
	}

	if (!DoSetDisplay())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool SetDisplaySettingsCommand::DoSetDisplay()
{
	SNRESULT snr;

	if (SN_FAILED(snr = SNPS3SetDisplaySettings(m_targetId,m_strBinary.c_str(),m_dwMonitor,m_dwConnector,m_dwResolution,m_bHDCP,m_bReset)))
	{
		PrintError(snr, L"SetDisplaySettings Failed!\n");
		return false;
	}

	return true;
}

void SetDisplaySettingsCommand::DisplayUsageHelp() const
{
	std::cout << "The setdisp command allows you to change the target's screen settings" << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl setdisp <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -monitor" << "\t" << "Monitor type" << std::endl;
	std::cout << "  -connector" << "\t" << "Connector type" << std::endl;
	std::cout << "  -res" << "\t\t" << "Startup resolution" << std::endl;
	std::cout << "  -hdcp" << "\t\t" << "High-bandwidth Digital Content Protection" << std::endl;
	std::cout << "  -reset" << "\t" << "Reset the target after the command" << std::endl;
	std::cout << "  -bin" << "\t\t" << "Path to custom setmonitor.self" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
