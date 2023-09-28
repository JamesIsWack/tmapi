/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef SETTINGS_COMMAND_H
#define SETTINGS_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class SettingsCommand : public TargetCommand
{
public:
	SettingsCommand();
	virtual			~SettingsCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoImport();
	bool			DoExport();
	virtual void	DisplayUsageHelp() const;

	std::string		m_strPath;
	enum {MODE_NONE=0,MODE_IMPORT,MODE_EXPORT} m_Mode;
};

TargetCommand* SettingsCommandFactory(void);

#endif