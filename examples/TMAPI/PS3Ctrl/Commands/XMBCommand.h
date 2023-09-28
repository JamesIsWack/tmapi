/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef XMB_COMMAND_H
#define XMB_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class XMBCommand : public TargetCommand
{
public:
					XMBCommand();
	virtual			~XMBCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	typedef enum	{CMD_NONE=-1,CMD_GET=0,CMD_SET} cmd_t;

	bool			DoSet();
	bool			DoGet();
	virtual void	DisplayUsageHelp() const;

	cmd_t			m_Command;

	std::string		m_strSetArgs;
	BOOL			m_bSync;
	BOOL			m_bReset;
};

TargetCommand* XMBCommandFactory(void);

#endif