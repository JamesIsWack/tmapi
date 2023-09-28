/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef PS3RUN_COMMAND_H
#define PS3RUN_COMMAND_H

#include "ConsoleCommand.h"

class PS3RunCommand : public ConsoleCommand
{
public:
	enum ResetMode { SOFT = 0, HARD = 1, QUICK = 2};

					PS3RunCommand();
	virtual			~PS3RunCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			ResetTargetFirst();
	bool			ProcessLoadELF();
	bool			GetResetParameters(RESET_PARAMETERS& rp);
	bool			SetBootParameters(RESET_PARAMETERS const& rp);
	bool			ProcessSDKCheck(void);
	virtual bool	WaitOnEvents();
	virtual void	DisplayUsageHelp() const;

	bool						m_bResetFirst;
	UINT64						m_bootParam;
	UINT64						m_bootMask;
	UINT64						m_resetParam;
	bool						m_performPowerCycle;
	bool						m_sdkCheck;
	bool						m_resetBootParams;
	RESET_PARAMETERS			m_originalResetParams;
	bool						m_bWaitForProcessExit;
	bool						m_bShowTM; // shows the TM on execute, false by default

	// Elf stuff
	std::string					m_elfPath;
	std::vector<std::string>	m_elfArgs;
	UINT32						m_modulePriority;
	UINT32						m_debugFlags;
	std::string					m_sfoDirectory;
	UINT64						m_threadId;
	UINT32						m_processId;

	static void __stdcall		TargetEventCallback(HTARGET hTarget, UINT uEventType, UINT /*uEvent*/, 
		SNRESULT snr, UINT uDataLen, BYTE *pData, void* /*pUser*/);
	static void					ProcessTargetEvent(HTARGET hTarget, UINT uDataLen, BYTE *pData);
	static void					ProcessTargetSpecificEvent(UINT uDataLen, BYTE *pData);
};

TargetCommand* PS3RunCommandFactory(void);

#endif