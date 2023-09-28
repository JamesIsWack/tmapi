/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include <conio.h>
#include "TargetCommand.h"
#include "StandardOption.h"
#include "SingleArgOption.h"
#include "MultiArgOption.h"

static const int MAX_LINE_SIZE = 1024;

class ConsoleCommand : public TargetCommand
{
public:
					ConsoleCommand();
	virtual			~ConsoleCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();
	
	int				GetConsoleAbortExitCode();
	bool			CheckForAbort();
	void			AbortOnTextMatch();
	virtual bool	WaitOnEvents();

protected:
	bool			InitializeTTYStreams();
	UINT32			ResolveTTYStreamName(std::string& streamName);
	bool			FindAbort(const wchar_t *pszText);
	bool			ProcessEvents();
	SNRESULT		ClearPendingMessages();
	void			UpdateTimeout();
	bool			ProcessRequestTTY();
	void			ClearMessagesBeforeExit(void);
	bool			DisconnectTTYEvents(void);
	char			GetChar();
	std::string		GetLine(bool& bEof);
	void			CheckTimeout();
	bool			SendTTY(const char* tty);
	bool			ProcessSendTTY(void);
	virtual void	DisplayUsageHelp() const;
	std::vector<std::string> GetTTYStreams();


	std::vector<UINT32>			m_bConsoleOutChannels;
	int							m_bConsoleInChannel;
	std::vector<std::string>	m_stdoutChannels;
	std::string					m_stdinChannel;
	std::wstring				m_abortString;
	bool						m_bConsoleIn;
	bool						m_bConsoleOut;
	std::vector<TTYSTREAM>		m_ttyStreams;
	int							m_abortExitCode;
	bool						m_bCheckForAbort;
	std::wstring				m_consoleBuffer;
	bool						m_bRelTimeoutSet;
	__time64_t					m_relTimeoutTime;
	bool						m_bAbsTimeoutSet;
	__time64_t					m_absTimeoutTime;
	UINT64						m_relTimeoutDuration;
	UINT64						m_absTimeoutDuration;
	bool						m_bReadStdInFromFile;
	FILE*						m_pConsoleImportSource;


public:
	static void __stdcall ProcessTTYCallback(HTARGET /*hTarget*/, UINT uiType, UINT uiStreamId, 
		SNRESULT /*Result*/, UINT uiLength, BYTE *pData, void* /*pUser*/);
};

TargetCommand* ConsoleCommandFactory(void);

#endif