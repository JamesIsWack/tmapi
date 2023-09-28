/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef FLASH_COMMAND_H
#define FLASH_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class FlashCommand : public TargetCommand
{
public:
					FlashCommand();
	virtual			~FlashCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();


protected:
	virtual void	DisplayUsageHelp() const;

	std::string		m_toolUpdaterPath;
	std::string		m_pupPath;
};

TargetCommand* FlashCommandFactory(void);

#endif