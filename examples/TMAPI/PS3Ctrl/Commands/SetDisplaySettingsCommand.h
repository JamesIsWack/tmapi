/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef SETDISPLAYSETTINGS_COMMAND_H
#define SETDISPLAYSETTINGS_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class SetDisplaySettingsCommand : public TargetCommand
{
public:
	SetDisplaySettingsCommand();
	virtual			~SetDisplaySettingsCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoSetDisplay();
	virtual void	DisplayUsageHelp() const;

	UINT32			m_dwMonitor;
	UINT32			m_dwConnector;
	UINT32			m_dwResolution;
	bool			m_bHDCP;
	bool			m_bReset;
	std::string		m_strBinary;
	bool			m_bArgsOk;

};

TargetCommand* SetDisplaySettingsCommandFactory(void);

#endif