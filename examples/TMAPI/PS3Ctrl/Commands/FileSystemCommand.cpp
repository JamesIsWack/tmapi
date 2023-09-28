/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "FileSystemCommand.h"

TargetCommand* MapFileSystemCommandFactory(void)
{
	return new MapFileSystemCommand();
}

MapFileSystemCommand::MapFileSystemCommand()
	: TargetCommand(false)
	, m_driveLetter(0)
	, m_bArgsOk(false)
{
}

MapFileSystemCommand::~MapFileSystemCommand()
{
}

bool MapFileSystemCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	m_cmdLineHandler.Parse(arguments);
	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
	m_bArgsOk = remainingArgs.size() == 1;

	if (m_bArgsOk)
	{
		m_driveLetter = remainingArgs[0].at(0);
	}

	m_cmdLineHandler.Reset();

	return true;
}

int MapFileSystemCommand::Run()
{
	int bRes = TargetCommand::Run();

	if (SN_FAILED(bRes))
		return bRes;

	if (!m_bArgsOk)
	{
		DisplayUsageHelp();
		return PS3CTRL_EXIT_ERROR;
	}

	if (!DoMapFileSystem())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool MapFileSystemCommand::DoMapFileSystem()
{
	SNRESULT snr;

	if (SN_FAILED(snr = SNPS3MapFileSystem(m_driveLetter)))
	{
		PrintError(snr, L"MapFileSystem Failed!\n");
		return false;
	}

	return true;
}

void MapFileSystemCommand::DisplayUsageHelp() const
{
	std::cout << "The map command allows you to map the Playstation3 file system" << std::endl << std::endl;

	std::cout << "Usage: 'PS3Ctrl map driveLetter'" << std::endl << std::endl;
	std::cout << "  Where driveLetter is the drive to be mapped to" << std::endl << std::endl;
}

TargetCommand* UnmapFileSystemCommandFactory(void)
{
	return new UnmapFileSystemCommand();
}

UnmapFileSystemCommand::UnmapFileSystemCommand()
	: TargetCommand(false)
	, m_bArgsOk(false)
{
}

UnmapFileSystemCommand::~UnmapFileSystemCommand()
{
}

bool UnmapFileSystemCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	m_cmdLineHandler.Parse(arguments);
	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
	m_bArgsOk = remainingArgs.empty();

	return true;
}

int UnmapFileSystemCommand::Run()
{
	int bRes = TargetCommand::Run();

	if (SN_FAILED(bRes))
		return bRes;

	if (!m_bArgsOk)
	{
		DisplayUsageHelp();
		return PS3CTRL_EXIT_ERROR;
	}

	if (!DoUnmapFileSystem())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool UnmapFileSystemCommand::DoUnmapFileSystem()
{
	SNRESULT snr;

	if (SN_FAILED(snr = SNPS3UnmapFileSystem()))
	{
		PrintError(snr, L"UnmapFileSystem Failed!\n");
		return false;
	}

	return true;
}

void UnmapFileSystemCommand::DisplayUsageHelp() const
{
	std::cout << "The unmap command allows you to unmap the Playstation3 file system" << std::endl << std::endl;

	std::cout << "Usage: 'PS3Ctrl unmap'" << std::endl << std::endl;
}

