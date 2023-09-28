/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "CaptureCommand.h"

TargetCommand* CaptureCommandFactory(void)
{
	return new CaptureCommand();
}

CaptureCommand::CaptureCommand()
: TargetCommand()
{

}

CaptureCommand::~CaptureCommand()
{

}

bool CaptureCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<UINT32> pid("pid", "process-id", INVALID_PROCESS);

	m_cmdLineHandler.AddArgument(pid);

	m_cmdLineHandler.Parse(arguments);

	m_processId = pid.GetValue();

	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
	if (remainingArgs.size() == 0)
		throw ArgumentException("You need to specify the output path for the image capture file!");
	else
		m_vramOutputPath = remainingArgs[0];

	return true;
}

int CaptureCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (m_processId == INVALID_PROCESS)
	{
		// Since user didn't specfy the target process ID, attempt to get it.

		if (!AutoGetProcessId(m_processId))
			return GetErrorCodeOnError();

		if (m_processId == INVALID_PROCESS)
		{
			PrintMessage(ML_WARN, L"No process was loaded and no PID was explicitly specified using the -pid option\n");
			return PS3CTRL_EXIT_OK;
		}
	}

	if (!DoVramCapture())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool CaptureCommand::AutoGetProcessId(UINT32 &processId) const
{
	processId = INVALID_PROCESS;
	UINT32 nProcessCount = 0;
	UINT32* pProcessListBuff = NULL;
	SNRESULT snr;
	
	// Get number of process IDs
	if (SN_FAILED(snr = SNPS3UserProcessList(m_targetId, &nProcessCount, NULL)))
	{
		PrintError(snr, L"Failed to get user process list!\n");
		return false;
	}

	if (nProcessCount > 0)
	{
		pProcessListBuff = new UINT32[nProcessCount];
		if (SN_FAILED(snr = SNPS3UserProcessList(m_targetId, &nProcessCount, pProcessListBuff)))
		{
			PrintError(snr, L"Failed to get user process list!\n");
			delete [] pProcessListBuff;
			return false;
		}

		processId = pProcessListBuff[nProcessCount - 1];
		delete [] pProcessListBuff;
	}

	return true;
}

bool CaptureCommand::DoVramCapture()
{
	_ASSERT(!m_vramOutputPath.empty() && m_processId != 0xffffffff);
	if (m_vramOutputPath.empty() || m_processId == 0xffffffff)
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3VRAMCapture(m_targetId, m_processId, NULL, m_vramOutputPath.c_str())))
	{
		PrintError(snr, L"Failed to do VRAM capture!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully done VRAM capture! Outputted to: %s\n",UTF8ToWChar(m_vramOutputPath).c_str());

	return true;
}

void CaptureCommand::DisplayUsageHelp() const
{
	std::cout << "The capture command allows you to perform an image capture." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl capture <options> <output_path>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -pid <hex>" << "\t" << "Process ID to perform image capture on (optional)" << std::endl;
	std::cout << std::endl;
	
	DisplayCommonOptions();
}
