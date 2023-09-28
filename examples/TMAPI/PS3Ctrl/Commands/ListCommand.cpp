/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "ListCommand.h"

TargetCommand* ListCommandFactory(void)
{
	return new ListCommand();
}

ListCommand::ListCommand()
: TargetCommand(false)
{

}

ListCommand::~ListCommand()
{
}

bool ListCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;

	return true;
}

int ListCommand::Run()
{
	int bRes = TargetCommand::Run();
	if (SN_FAILED(bRes))
		return bRes;

	if (!ShowTargetList())
		return GetErrorCodeOnError();

	std::vector<SNPS3TargetInfo*>::iterator iter = ms_Targets.begin();

	return m_exitCode;
}

void ListCommand::DisplayUsageHelp() const
{
	std::cout << "The list command lists all added targets." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl list" << std::endl << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}

bool ListCommand::ShowTargetList()
{
	SNRESULT snr = 0;
	if (SN_FAILED( snr = SNPS3EnumerateTargets(EnumCallBack) ))
	{
		PrintError(snr, L"Failed to enumerate targets\n");
		return false;
	}

	if (ms_Targets.size() == 0)
	{
		PrintError(SN_E_NO_TARGETS, L"No targets added\n");
		return false;
	}

	std::vector<SNPS3TargetInfo*>::iterator iter = ms_Targets.begin();

	HTARGET defaultTargetId = INVALID_TARGET;
	snr = SNPS3GetDefaultTarget(&defaultTargetId);

	if (SN_FAILED(snr) || defaultTargetId == INVALID_TARGET)
	{
		PrintMessage(ML_WARN, L"Failed to retrieve default target\n");
	}

	while (iter != ms_Targets.end())
	{
		std::string usage("Unknown");
		char*  pszUsage = 0;
		ECONNECTSTATUS nStatus = (ECONNECTSTATUS) -1;
		snr = SNPS3GetConnectStatus((*iter)->hTarget, &nStatus, &pszUsage);
		if (SN_SUCCEEDED(snr))
		{
			switch(nStatus)
			{
			case CS_IN_USE: 
				usage = std::string(pszUsage);
				break;
			case CS_CONNECTED:
				usage = std::string("Connected");
				break;
			case CS_NOT_CONNECTED:
				usage = std::string("Not connected");
				break;
			case CS_UNAVAILABLE:
				usage = std::string("Unavailable");
			default:
				break;            
			}
		}

		SNPS3TargetInfo info;
		info.hTarget = (*iter)->hTarget;
		info.nFlags = SN_TI_TARGETID | SN_TI_NAME | SN_TI_INFO;
		snr = SNPS3GetTargetInfo(&info);
		if (SN_FAILED(snr))
		{
			PrintError(ML_WARN, L"No connection info available for target id=%d", (*iter)->hTarget);
			return false;
		}

		std::cout << "- " << std::endl;
		std::cout << " Default: " << (((*iter)->hTarget == defaultTargetId) ? "True" : "False") << std::endl;

		TMAPI_TCPIP_CONNECT_PROP oConnection;
		snr = SNPS3GetConnectionInfo((*iter)->hTarget, &oConnection);
		if (SN_SUCCEEDED(snr))
		{
			std::cout << " Host: " << oConnection.szIPAddress << ":" << oConnection.uPort << std::endl;
		}

		std::cout << " Name: " << info.pszName << std::endl;
		std::cout << " Type: " << info.pszType << std::endl;
		std::cout << " Connection Status: " << usage << std::endl;

		++iter;
	}

	return true;
}
