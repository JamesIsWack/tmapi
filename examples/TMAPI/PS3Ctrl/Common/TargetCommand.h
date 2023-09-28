/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef TARGET_COMMAND_H
#define TARGET_COMMAND_H

#include "CommandLineTools.h"
#include "ps3tmapi.h"
#include "tmver.h"
#include "Defines.h"
#include <windows.h>
#include <memory>
#include "Argument.h"

using namespace commandargutils;

struct RESET_PARAMETERS
{
	struct Parameter
	{
		UINT64	uValue;
		UINT64	uMask;
	};

	struct Parameter Reset;
	struct Parameter Boot;
	struct Parameter System;
};

struct TARGET_ADD_DATA
{
	std::string TargetName;
	std::string	TargetType;
	std::string Hostname;
	UINT32		Port;
};

class TargetCommand
{
public:

					TargetCommand(bool bConnectToTarget = true);
	virtual			~TargetCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();
	virtual int		InitTargetComms();
	virtual bool	Init();
	virtual void	DisplayUsageVersionAndCopyright() const;
	virtual void	DisplayUsageHelp() const;

	virtual bool	WaitOnEvents()
	{
		return false;
	}
	void SetTargetName(std::string targetName)
	{ 
		m_targetName = targetName; 
	}
	void SetTargetId(HTARGET hTargetId) 
	{ 
		m_targetId = hTargetId; 
	}
	bool SurpressErrorLogging() const
	{
		return m_bSurpressErrorLogging; 
	}
	std::string& GetName()
	{ 
		return m_targetName; 
	}
	std::string& GetLoggingPath() 
	{ 
		return m_logFilePath; 
	}
	bool LoggingToFileEnabled() const
	{ 
		return m_bLogToFile; 
	}
	void SetAbortKick() 
	{ 
		m_bAbortKick  = true; 
	}
	bool AbortKick() const
	{ 
		return m_bAbortKick; 
	}
	int GetErrorCodeOnError() const
	{
		return m_exitCode ? m_exitCode : PS3CTRL_EXIT_ERROR;
	}

protected:
	virtual void	OnClose();
	bool			AddTargets(HTARGET& firstTargetId);
	bool			DeleteTargets();
	bool			SetDefaultTarget();
	bool			SetUpTarget();
	bool			ConnectToActiveTarget();
	bool			SetFSDir();
	bool			SetHomeDir();
	bool			ExtractTargetDetails(std::string& str, TARGET_ADD_DATA& data);
	void			SetExitCode(UINT32 exitCode);
	void			DisplayCommonOptions() const;
	void			ShowUsage() const;

	commandargutils::CommandLineHandler m_cmdLineHandler;

	// Common options
	std::string						m_targetName;
	bool							m_bLaunchTargetPicker;
	bool							m_bAlwaysDC;
	bool							m_bDCifNotConnected;
	bool							m_bForceDC;
	bool							m_bWasOriginallyConnected;
	HTARGET							m_targetId;
	bool							m_bLogToFile;
	std::string						m_logFilePath;
	bool							m_bSurpressErrorLogging;
	bool							m_bAbortKick;
	std::vector<std::string>		m_deleteTargetsNames;
	std::vector<TARGET_ADD_DATA>	m_addTargetsVect;
	std::string						m_fileServingPath;
	std::string						m_homeDirectoryPath;
	UINT32							m_abortErrorCode;
	UINT32							m_exitCode;
	bool							m_bConnectToTarget;
	std::string						m_defaultTarget;

public:
	static int __stdcall EnumCallBack(HTARGET hTarget);
	static bool TargetCommand::FindFirstConnectedTarget(void);
	static bool FindFirstAvailableTarget(void);
	static void PrintError(SNRESULT snr, const WCHAR* pszMessage, ...);
	static void PrintMessage(MSG_LEVEL nMsgLevel, const WCHAR *pszFormat, ...);
	static bool GetTargetFromAddress(const char *pszIPAddr, HTARGET& hTarget);
	static std::string GetTargetType(std::string& str);
	static bool GetHostnames(const char* input, std::string& ipOut, std::string& dnsNameOut);

	static TargetCommand					*ms_TargetCommandObj;
	static std::vector<SNPS3TargetInfo*>	ms_Targets;

};

typedef TargetCommand* (*CommandFactory) (void);

TargetCommand* TargetCommandFactory(void);

class CommandType : public CommandArgument
{
public:
	CommandType(const std::string& name, CommandFactory func)
		: CommandArgument(name)
		, m_pCommandFactory(func) {};

	TargetCommand*	CreateCommand()
	{
		ATLASSERT(m_pCommandFactory);
		return m_pCommandFactory();
	}

protected:
	CommandFactory	m_pCommandFactory;
};
typedef std::vector<CommandType> VecCommandType;

extern VecCommandType g_Commands;

#endif