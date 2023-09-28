/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "SetFlagsCommand.h"

TargetCommand* SetFlagsCommandFactory(void)
{
	return new SetFlagsCommand();
}

SetFlagsCommand::SetFlagsCommand()
: TargetCommand()
, m_screenCaptureFlags(0)
, m_coreDumpFlags(0)
, m_extraLoadFlags(0)
, m_extraLoadFlagsMask(0)
{

}

SetFlagsCommand::~SetFlagsCommand()
{

}

bool SetFlagsCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<UINT64> cdf("cdf", "core-dump-flags", 0, false);
	cdf.SetTaskId(SET_CORE_DUMP_FLAGS);
	m_cmdLineHandler.AddArgument(cdf);

	StandardOption es("es", "enable-screen-capture");
	es.SetTaskId(ENABLE_SCREEN_CAPTURE);
	m_cmdLineHandler.AddArgument(es);

	SingleArgOption<UINT64> sf("sf", "set-screen-capture-flags", 0);
	sf.SetTaskId(SET_SCREEN_CAPTURE_FLAGS);
	m_cmdLineHandler.AddArgument(sf);

	SingleArgOption<UINT64> xlf("xlf", "set-extra-load-flags", 0);
	xlf.SetTaskId(SET_EXTRA_LOAD_FLAGS);
	m_cmdLineHandler.AddArgument(xlf);

	SingleArgOption<UINT64> xlfm("xlfm", "set-extra-load-flags-mask", 0);
	xlfm.SetParentDependency(&xlf);
	m_cmdLineHandler.AddArgument(xlfm);

	m_cmdLineHandler.Parse(arguments);

	if (cdf.IsPassed())
	{
		m_tasks.push_back(static_cast<Task>(cdf.GetTaskId()));

		if (cdf.IsPassed())
			m_coreDumpFlags = cdf.GetValue();
	}

	if (es.IsPassed())
		m_tasks.push_back(static_cast<Task>(es.GetTaskId()));

	if (sf.IsPassed())
	{
		m_tasks.push_back(static_cast<Task>(sf.GetTaskId()));
		m_screenCaptureFlags = sf.GetValue();
	}

	if (xlf.IsPassed())
	{
		m_tasks.push_back(static_cast<Task>(xlf.GetTaskId()));
		m_extraLoadFlags = xlf.GetValue();

		if (xlfm.IsPassed())
			m_extraLoadFlagsMask = xlfm.GetValue();
	}
	
	return true;
}

int SetFlagsCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	std::vector<Task>::const_iterator it = m_tasks.begin();

	if (it == m_tasks.end())
	{
		DisplayUsageHelp();
		return PS3CTRL_EXIT_OK;
	}

	for (; it != m_tasks.end(); ++it)
	{
		switch(*it)
		{
		case SET_SCREEN_CAPTURE_FLAGS:
			if (!SetScreenCaptureFlags())
				return GetErrorCodeOnError();
			break;
		case ENABLE_SCREEN_CAPTURE:
			if (!EnableScreenCapture())
				return GetErrorCodeOnError();
			break;
		case SET_CORE_DUMP_FLAGS:
			if (!SetCoreDumpFlags())
				return GetErrorCodeOnError();
			break;
		case SET_EXTRA_LOAD_FLAGS:
			if (!SetExtraLoadFlags())
				return GetErrorCodeOnError();
			break;
		}
	}

	return m_exitCode;
}

bool SetFlagsCommand::SetScreenCaptureFlags()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3SetVRAMCaptureFlags(m_targetId, m_screenCaptureFlags)))
	{
		PrintError(snr, L"Error setting VRAM capture flags [0x%llX]!\n", m_screenCaptureFlags);
		return false;
	}

	PrintMessage(ML_INFO, L"Set VRAM capture flags [0x%llX]\n", m_screenCaptureFlags);

	return true;
}

bool SetFlagsCommand::EnableScreenCapture()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3EnableVRAMCapture(m_targetId)))
	{
		PrintError(snr, L"Error enabling VRAM capture!\n");
		return false;
	}

	PrintMessage(ML_INFO, L"Enabled VRAM Capture!\n");

	return true;
}

bool SetFlagsCommand::SetCoreDumpFlags()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3SetCoreDumpFlags(m_targetId, m_coreDumpFlags)))
	{
		PrintError(snr, L"Error setting core dump flags [0x%llX]\n", m_coreDumpFlags);
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully set core dump flags [0x%llX]\n", m_coreDumpFlags);
	return true;
}


bool SetFlagsCommand::SetExtraLoadFlags()
{
	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3SetExtraLoadFlags(m_targetId, m_extraLoadFlags, m_extraLoadFlagsMask)))
	{
		PrintError(snr, L"Error setting extra load flags [0x%llX] with a mask of [0x%llX]\n", m_extraLoadFlags, m_extraLoadFlagsMask);
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully set extra load flags [0x%llX] with a mask of [0x%llX]\n", m_extraLoadFlags, m_extraLoadFlagsMask);
	return true;
}


void SetFlagsCommand::DisplayUsageHelp() const
{
	std::cout << "The set-flags command allows you set various flags on the target." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl set-flags <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;

	std::cout << "  -cdf <hex>" << "\t" << "Set the core dump flags" << std::endl;
	std::cout << "  -es" << "\t\t" << "Enables image capture" << std::endl;
	std::cout << "  -sf <hex>" << "\t" << "Set image capture flags" << std::endl;
	std::cout << "  -xlf <hex>" << "\t" << "Set extra load flags" << std::endl;
	std::cout << "    -xlfm <hex>" << "\t" << "Set extra load flags mask" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
