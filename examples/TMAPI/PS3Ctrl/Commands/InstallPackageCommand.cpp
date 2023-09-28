/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "InstallPackageCommand.h"

TargetCommand* InstallPackageCommandFactory(void)
{
	return new InstallPackageCommand();
}

InstallPackageCommand::InstallPackageCommand()
: TargetCommand()
{

}

InstallPackageCommand::~InstallPackageCommand()
{

}

bool InstallPackageCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;


	m_cmdLineHandler.Parse(arguments);

	std::vector<std::string>& remainingArgs = m_cmdLineHandler.GetRemainingArguments();
	if (remainingArgs.size() == 0)
		throw ArgumentException("You need to specify the package file path!");
	else
		m_pkgPath = remainingArgs[0];

	return true;
}

int InstallPackageCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (!DoInstallPackage())
		return GetErrorCodeOnError();

	return m_exitCode;
}

bool InstallPackageCommand::DoInstallPackage()
{
	_ASSERT(!m_pkgPath.empty());
	if (m_pkgPath.empty())
		return false;

	SNRESULT snr;
	if (SN_FAILED(snr = SNPS3InstallPackage(m_targetId, m_pkgPath.c_str())))
	{
		PrintError(snr, L"Error installing package. Package path supplied:%s\n", UTF8ToWChar(m_pkgPath).c_str());
		return false;
	}

	PrintMessage(ML_INFO, L"Successfully installed package");

	return true;
}

void InstallPackageCommand::DisplayUsageHelp() const
{
	std::cout << "The install-package command allows you to install a package." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl install-package <options> <file>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
