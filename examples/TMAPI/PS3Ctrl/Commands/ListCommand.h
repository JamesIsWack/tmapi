/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef LIST_COMMAND_H
#define LIST_COMMAND_H

#include "TargetCommand.h"

class ListCommand : public TargetCommand
{
public:
	ListCommand();
	virtual			~ListCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	virtual void	DisplayUsageHelp() const;
	bool			ShowTargetList();
};

TargetCommand* ListCommandFactory(void);

#endif