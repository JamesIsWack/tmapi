/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "ProcessCommand.h"

TargetCommand* ProcessCommandFactory(void)
{
	return new ProcessCommand();
}

ProcessCommand::ProcessCommand()
: TargetCommand()
, m_terTimeout(0)
, m_processId(INVALID_PROCESS)
{

}

ProcessCommand::~ProcessCommand()
{

}

bool ProcessCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<UINT32> kp("kp", "kill", INVALID_PROCESS);
	kp.SetTaskId(KILL_PROCESS);
	SingleArgOption<UINT32> sp("sp", "stop", INVALID_PROCESS);
	sp.SetTaskId(STOP_PRCESS);
	SingleArgOption<UINT32> cp("cp", "continue", INVALID_PROCESS);
	cp.SetTaskId(CONTINUE_PROCESS);
	SingleArgOption<UINT32> tp("tp", "terminate", INVALID_PROCESS);
	tp.SetTaskId(TERMINATE_PROCESS);

	SingleArgOption<UINT32> tt("tt", "terminate-timeout", 0, false);
	tt.SetParentDependency(&tp);

	m_cmdLineHandler.AddArgument(kp);
	m_cmdLineHandler.AddArgument(sp);
	m_cmdLineHandler.AddArgument(cp);
	m_cmdLineHandler.AddArgument(tp);
	m_cmdLineHandler.AddArgument(tt);

	m_cmdLineHandler.Parse(arguments);

	if (kp.IsPassed())
	{
		m_task = static_cast<Task>(kp.GetTaskId());
		m_processId = kp.GetValue();
	}
	else if (sp.IsPassed())
	{
		m_task = static_cast<Task>(sp.GetTaskId());
		m_processId = sp.GetValue();
	}
	else if (cp.IsPassed())
	{
		m_task = static_cast<Task>(cp.GetTaskId());
		m_processId = cp.GetValue();
	}
	else if (tp.IsPassed())
	{
		m_task = static_cast<Task>(tp.GetTaskId());
		m_processId = tp.GetValue();
		if (tt.IsPassed())
			m_terTimeout = tt.GetValue();
	}

	return true;
}

int ProcessCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	switch(m_task)
	{
	case STOP_PRCESS:
		if (!StopProcess())
			return GetErrorCodeOnError();
		break;
	case CONTINUE_PROCESS:
		if (!ContinueProcess())
			return GetErrorCodeOnError();
		break;
	case KILL_PROCESS:
		if (!KillProcess())
			return GetErrorCodeOnError();
		break;
	case TERMINATE_PROCESS:
		if (!TerminateProcess())
			return GetErrorCodeOnError();
		break;
	default:
		DisplayUsageHelp();
		break;
	}

	return m_exitCode;
}

bool ProcessCommand::StopProcess()
{
	_ASSERT(m_processId != 0xffffffff);
	if (m_processId == 0xffffffff)
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3ProcessStop(m_targetId, m_processId)))
	{
		PrintError(snr, L"Error stopping process!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully Stopped Process ID=0x%X\n", m_processId);

	return true;
}

bool ProcessCommand::ContinueProcess()
{
	_ASSERT(m_processId != 0xffffffff);
	if (m_processId == 0xffffffff)
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3ProcessContinue(m_targetId, m_processId)))
	{
		PrintError(snr, L"Error continuing process!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully Continued Process ID=0x%X\n", m_processId);

	return true;
}

bool ProcessCommand::KillProcess()
{
	_ASSERT(m_processId != 0xffffffff);
	if (m_processId == 0xffffffff)
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3ProcessKill(m_targetId, m_processId)))
	{
		PrintError(snr, L"Error killing process!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully Killed Process ID=0x%X\n", m_processId);

	return true;
}

bool ProcessCommand::TerminateProcess()
{
	_ASSERT(m_processId != 0xffffffff);
	if (m_processId == 0xffffffff)
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3TerminateGameProcess(m_targetId, m_processId, m_terTimeout)))
	{
		PrintError(snr, L"Error terminating process!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully Terminated Process ID=0x%X\n", m_processId);

	return true;
}

void ProcessCommand::DisplayUsageHelp() const
{
	std::cout << "The process command allows you kill,terminate, stop or continue a process." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl process <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	
	std::cout << "  -sp <pid>" << "\t" << "Stops the process with process id of <pid> (hex)" << std::endl;
	std::cout << "  -cp <pid>" << "\t" << "Continues the process with process id of <pid> (hex)" << std::endl;
	std::cout << "  -kp <pid>" << "\t" << "Kills all threads in the process with process id of <pid> (hex)" << std::endl;
	std::cout << "  -tp <pid>" << "\t" << "Terminates the process with process id of <pid> (hex)" << std::endl;
	std::cout << "   -tt <timeout>" << "\t" << "Timeout (ms) to use for terminate process" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
