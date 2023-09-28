/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef INSTALL_PACKAGE_COMMAND_H
#define INSTALL_PACKAGE_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class InstallPackageCommand : public TargetCommand
{
public:
					InstallPackageCommand();
	virtual			~InstallPackageCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoInstallPackage();
	virtual void	DisplayUsageHelp() const;

	std::string		m_pkgPath;
};

TargetCommand* InstallPackageCommandFactory(void);

#endif