/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "DeleteCommand.h"

DeleteCommand::DeleteCommand(void) : m_timeoutValue(-1)
{
}

DeleteCommand::~DeleteCommand(void)
{
}

TargetCommand *DeleteCommandFactory(void)
{
	return new DeleteCommand();
}

bool DeleteCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	SingleArgOption<int> to("to", "timeout", -1);
	m_cmdLineHandler.AddArgument(to);
	m_cmdLineHandler.Parse(arguments);
	m_timeoutValue = to.GetValue();

	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
		
	if (remainingArgs.size() == 0)
	{
		throw ArgumentException("You must specify at least one file or directory to delete!");
	}

	std::vector<std::string>::const_reverse_iterator it = remainingArgs.rbegin();
	for (; it != remainingArgs.rend(); ++it)
	{
		m_filesToDelete.push_back(*it);
	}

	return true;
}

int DeleteCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (!DoDeleteFile())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool DeleteCommand::DoDeleteFile()
{
	SNRESULT snr;
	std::vector<std::string>::const_reverse_iterator it = m_filesToDelete.rbegin();
	for (; it != m_filesToDelete.rend(); ++it)
	{
		snr = SNPS3DeleteEx(m_targetId, (*it).c_str(), m_timeoutValue);
		if (!SN_SUCCEEDED(snr))
		{
			if (snr == SN_E_FILE_ERROR)
				PrintError(snr, UTF8ToWChar(std::string("Error - Cannot find " + *(it) + " on target.")).c_str());
			else if (snr == SN_E_COMMS_ERR)
			{
				PrintError(snr, UTF8ToWChar(std::string("Error - Problem communicating with target.")).c_str());
				return false;
			}
			else
			{
				PrintError(snr, UTF8ToWChar(std::string("Error - Failed to delete target files or directories.")).c_str());
				return false;
			}
		}
	}

	return true;
}
