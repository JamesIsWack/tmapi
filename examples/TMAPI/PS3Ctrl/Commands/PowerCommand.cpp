/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "PowerCommand.h"

TargetCommand* PowerCommandFactory(void)
{
	return new PowerCommand();
}

PowerCommand::PowerCommand()
: TargetCommand()
{

}

PowerCommand::~PowerCommand()
{

}

bool PowerCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	StandardOption pon("on", "power-on");
	pon.SetTaskId(POWER_ON);
	StandardOption pof("off", "power-off");
	pof.SetTaskId(POWER_OFF);
	StandardOption foff("foff", "force-power-off");
	foff.SetTaskId(FORCE_POWER_OFF);

	m_cmdLineHandler.AddArgument(pon);
	m_cmdLineHandler.AddArgument(pof);
	m_cmdLineHandler.AddArgument(foff);

	m_cmdLineHandler.Parse(arguments);

	if (pon.IsSet())
		m_task = static_cast<Task>(pon.GetTaskId());
	else if (pof.IsSet())
		m_task = static_cast<Task>(pof.GetTaskId());
	else if (foff.IsSet())
		m_task = static_cast<Task>(foff.GetTaskId());

	return true;
}

int PowerCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	switch(m_task)
	{
	case POWER_ON:
		if (!PerformPowerOn())
			return GetErrorCodeOnError();
		break;
	case POWER_OFF:
		if (!PerformPowerOff())
			return GetErrorCodeOnError();
		break;
	case FORCE_POWER_OFF:
		if (!PerformPowerOff(true))
			return GetErrorCodeOnError();
		break;
	default:
		DisplayUsageHelp();
		break;
	}

	return m_exitCode;
}

bool PowerCommand::PerformPowerOff(bool bForcePowerOff)
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3PowerOff(m_targetId, bForcePowerOff ? 1 : 0)))
	{
		PrintError(snr, L"Failed to power off target. Used force: %d", bForcePowerOff);
		return false;
	}

	PrintMessage(ML_INFO, L"Powered off target. Used force: %d", bForcePowerOff);
	return true;
}

bool PowerCommand::PerformPowerOn()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3PowerOn(m_targetId)))
	{
		PrintError(snr, L"Failed to power on target\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Powered on target");
	return true;
}

void PowerCommand::DisplayUsageHelp() const
{
	std::cout << "The power command allows you to power on and power off a target." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl power <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -on" << "\t\t" << "Powers on the target" << std::endl;
	std::cout << "  -off" << "\t\t" << "Powers off the target" << std::endl;
	std::cout << "  -foff" << "\t\t" << "Does a force power off on the target" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
