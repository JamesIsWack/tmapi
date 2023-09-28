/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef DELETE_COMMAND_H
#define DELETE_COMMAND_H

#include "TargetCommand.h"

class DeleteCommand : public TargetCommand
{
public:
					DeleteCommand(void);
	virtual			~DeleteCommand(void);
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoDeleteFile();

	std::vector<std::string> m_filesToDelete;
	int m_timeoutValue;

};

TargetCommand *DeleteCommandFactory(void);

#endif