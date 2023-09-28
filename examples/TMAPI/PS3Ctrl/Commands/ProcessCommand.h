/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_COMMAND_H
#define PROCESS_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"
#include "StandardOption.h"

class ProcessCommand : public TargetCommand
{
public:
	enum Task 
	{	
		STOP_PRCESS, CONTINUE_PROCESS,
		KILL_PROCESS, TERMINATE_PROCESS
	};

						ProcessCommand();
	virtual				~ProcessCommand();
	virtual bool		ParseArgs(std::vector<std::string>& arguments);
	virtual int			Run();

protected:
	bool				StopProcess();
	bool				ContinueProcess();
	bool				KillProcess();
	bool				TerminateProcess();
	virtual void		DisplayUsageHelp() const;

	UINT32		m_terTimeout;
	Task		m_task;
	UINT32		m_processId;
};

TargetCommand* ProcessCommandFactory(void);

#endif