/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "CoreDumpCommand.h"

TargetCommand* CoreDumpCommandFactory(void)
{
	return new CoreDumpCommand();
}

CoreDumpCommand::CoreDumpCommand()
: TargetCommand()
, m_userData1(0)
, m_userData2(0)
, m_userData3(0)
, m_processId(INVALID_PROCESS)
{

}

CoreDumpCommand::~CoreDumpCommand()
{

}

bool CoreDumpCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<UINT32> pid("pid", "process-id", INVALID_PROCESS, true);
	MultiArgOption<UINT64> ud("ud", "user-data", false, 3);

	m_cmdLineHandler.AddArgument(pid);
	m_cmdLineHandler.AddArgument(ud);

	m_cmdLineHandler.Parse(arguments);

	m_processId = pid.GetValue();

	if (ud.IsPassed())
	{
		std::vector<UINT64>& userData = ud.GetValues();
		m_userData1 = userData[0];
		m_userData2 = userData[1];
		m_userData3 = userData[2];
	}

	return true;
}

int CoreDumpCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (!DoTriggerCoreDump())
		return GetErrorCodeOnError();

	return bRes;
}

bool CoreDumpCommand::DoTriggerCoreDump()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3TriggerCoreDump(m_targetId, m_processId, m_userData1, m_userData2, m_userData3)))
	{
		PrintError(snr, L"Failure to trigger core dump\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully triggered core dump\n");
	
	return true;
}

void CoreDumpCommand::DisplayUsageHelp() const
{
	std::cout << "The coredump command allows you to perform a coredump on a process." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl coredump <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -pid <hex>" << "\t" << "Process ID to do the dump on" << std::endl;
	std::cout << "  -ud x y z" << "\t" << "User data 1,2 and 3 in hex for core dump" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
