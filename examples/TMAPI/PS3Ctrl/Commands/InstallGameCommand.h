/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef INSTALL_GAME_COMMAND_H
#define INSTALL_GAME_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class InstallGameCommand : public TargetCommand
{
public:
					InstallGameCommand();
	virtual			~InstallGameCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoInstallGame();
	std::string		GetFTText(TMAPI_FT_NOTIFY& val);
	virtual void	DisplayUsageHelp() const;

	std::string		m_sfoPath;
	bool			m_bWaitForTransfer;
	UINT32			m_waitTimeout;
};

TargetCommand* InstallGameCommandFactory(void);

#endif