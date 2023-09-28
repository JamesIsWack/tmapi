/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef CORE_DUMP_COMMAND_H
#define CORE_DUMP_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class CoreDumpCommand : public TargetCommand
{
public:
					CoreDumpCommand();
	virtual			~CoreDumpCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoTriggerCoreDump();
	virtual void	DisplayUsageHelp() const;

	UINT64			m_userData1;
	UINT64			m_userData2;
	UINT64			m_userData3;
	UINT32			m_processId;
};

TargetCommand* CoreDumpCommandFactory(void);

#endif