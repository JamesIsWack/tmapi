/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include "CommandLineTools.h"
#include "ConsoleCommand.h"
#include "FlashCommand.h"
#include "PowerCommand.h"
#include "SyncCommand.h"
#include "PS3RunCommand.h"
#include "ProcessCommand.h"
#include "CaptureCommand.h"
#include "FormatCommand.h"
#include "InstallGameCommand.h"
#include "InstallPackageCommand.h"
#include "CoreDumpCommand.h"
#include "SetFlagsCommand.h"
#include "BDCommand.h"
#include "XMBCommand.h"
#include "SetDisplaySettingsCommand.h"
#include "FileSystemCommand.h"
#include "SettingsCommand.h"
#include "DeleteCommand.h"
#include "ListCommand.h"

using namespace commandargutils;

// Function prototypes
int RunPs3Ctrl(std::vector<std::string>& arguments);
int ExecuteTargetCommand(TargetCommand *pTargetCommandObj, std::vector<std::string>& arguments);

int wmain(int argc, WCHAR *argv[])
{
	// Convert the widechars to UTF8 strings
	char** pszArgv = NULL;

	std::vector<std::string> argvVect;
	for(int i = 0; i < argc; ++i)
	{
		std::string arg;
		if (!WCharToUTF8(argv[i], arg))
		{
			std::cerr << "Error handling command line arguments" << std::endl;
			return PS3CTRL_EXIT_ERROR;
		}
		argvVect.push_back(arg);
	}

	return RunPs3Ctrl(argvVect);
}

int RunPs3Ctrl(std::vector<std::string>& arguments)
{
	int retVal = PS3CTRL_EXIT_OK;

	// Add your commands here...
	g_Commands.push_back(CommandType("console"			, ConsoleCommandFactory));
	g_Commands.push_back(CommandType("run"				, PS3RunCommandFactory));
	g_Commands.push_back(CommandType("flash"			, FlashCommandFactory));
	g_Commands.push_back(CommandType("power"			, PowerCommandFactory));
	g_Commands.push_back(CommandType("sync"				, SyncCommandFactory));
	g_Commands.push_back(CommandType("process"			, ProcessCommandFactory));
	g_Commands.push_back(CommandType("capture"			, CaptureCommandFactory));
	g_Commands.push_back(CommandType("format"			, FormatCommandFactory));
	g_Commands.push_back(CommandType("install-game"		, InstallGameCommandFactory));
	g_Commands.push_back(CommandType("install-package"	, InstallPackageCommandFactory));
	g_Commands.push_back(CommandType("coredump"			, CoreDumpCommandFactory));
	g_Commands.push_back(CommandType("set-flags"		, SetFlagsCommandFactory));
	g_Commands.push_back(CommandType("bdemu"			, BDCommandFactory));
	g_Commands.push_back(CommandType("xmb"				, XMBCommandFactory));
	g_Commands.push_back(CommandType("setdisp"			, SetDisplaySettingsCommandFactory));
	g_Commands.push_back(CommandType("map"				, MapFileSystemCommandFactory));
	g_Commands.push_back(CommandType("unmap"			, UnmapFileSystemCommandFactory));
	g_Commands.push_back(CommandType("settings"			, SettingsCommandFactory));
	g_Commands.push_back(CommandType("delete"			, DeleteCommandFactory));
	g_Commands.push_back(CommandType("list"				, ListCommandFactory));

	arguments.erase(arguments.begin()); // remove the program name from command line args

	// First check the category
	CommandLineHandler commandLineHandler;
	for (size_t i = 0; i < g_Commands.size() ; i++)
	{
		commandLineHandler.AddArgument(g_Commands[i]);
	}

	try
	{
		commandLineHandler.Parse(arguments);
	}
	catch(std::exception &e)
	{
		std::cerr << "Internal Error: " << e.what() << std::endl;
		return PS3CTRL_EXIT_ERROR;
	}

	TargetCommand* pTargetCommandObj = NULL;
	for (size_t i = 0; i < g_Commands.size() ; i++)
	{
		if (g_Commands[i].IsSet())
		{
			pTargetCommandObj = g_Commands[i].CreateCommand();
			break;
		}
	}

	if (!pTargetCommandObj)
	{
		pTargetCommandObj = new TargetCommand(false);
		if (!pTargetCommandObj)
		{
			std::cerr << "Internal Error: Failed to create command" << std::endl;
			return PS3CTRL_EXIT_ERROR;
		}

		if (arguments.empty())
		{
			arguments.push_back("help");
		}
	}

	retVal = ExecuteTargetCommand(pTargetCommandObj, arguments);
	delete pTargetCommandObj;
	
	return retVal;
}

int ExecuteTargetCommand(TargetCommand *pTargetCommandObj, std::vector<std::string>& arguments)
{
	_ASSERT(pTargetCommandObj);
	if (!pTargetCommandObj)
		return PS3CTRL_EXIT_ERROR;

	try
	{
		if (!pTargetCommandObj->ParseArgs(arguments))
			return PS3CTRL_EXIT_OK; // Probably displayed help.
	}
	catch(std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return PS3CTRL_EXIT_ERROR;
	}

	return pTargetCommandObj->Run();
}