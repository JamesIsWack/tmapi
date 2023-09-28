/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef FORMAT_COMMAND_H
#define FORMAT_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class FormatCommand : public TargetCommand
{
public:
					FormatCommand();
	virtual			~FormatCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoFormatHDD();
	virtual void	DisplayUsageHelp() const;

	bool			m_bClearReg;
};

TargetCommand* FormatCommandFactory(void);

#endif