/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "XMBCommand.h"

TargetCommand* XMBCommandFactory(void)
{
	return new XMBCommand();
}

XMBCommand::XMBCommand()
: TargetCommand(),
m_Command(CMD_NONE),
m_bSync(FALSE),
m_bReset(FALSE)
{

}

XMBCommand::~XMBCommand()
{

}

bool XMBCommand::ParseArgs(std::vector<std::string>& arguments)
{
	if (!TargetCommand::ParseArgs(arguments))
		return false;


	SingleArgOption<std::string> set("set", "set", "");
	StandardOption get("get", "get");
	StandardOption sync("sync", "sync");
	StandardOption reset("reset", "reset");

	m_cmdLineHandler.AddArgument(set);
	m_cmdLineHandler.AddArgument(get);
	m_cmdLineHandler.AddArgument(sync);
	m_cmdLineHandler.AddArgument(reset);

	m_cmdLineHandler.Parse(arguments);

	if (sync.IsPassed())
	{
		m_bSync = TRUE;
	}

	if (reset.IsPassed())
	{
		m_bReset = TRUE;
	}
	
	if (set.IsPassed())
	{
		m_Command = XMBCommand::CMD_SET;
		m_strSetArgs = set.GetValue();
	}
	else if (get.IsPassed())
	{
		m_Command = XMBCommand::CMD_GET;
	}

	m_cmdLineHandler.Reset();
	return true;
}

int XMBCommand::Run()
{
	int bRes = TargetCommand::Run();

	if (SN_FAILED(bRes))
		return bRes;

	switch(m_Command)
	{
		case CMD_SET:
			if (!DoSet())
				return GetErrorCodeOnError();
			break;
		case CMD_GET:
			if (!DoGet())
				return GetErrorCodeOnError();
			break;
		default:
			DisplayUsageHelp();
			return PS3CTRL_EXIT_ERROR;
	}

	return m_exitCode;
}

bool XMBCommand::DoGet()
{
	SNRESULT snr = 0;
	UINT uSize = 4097;
	char szBuf[4097]; //This is the maximum the server can allocate plus one for the \0
	//TODO: Would be nice if the API returns the required buffer size if a NULL buffer pointer is passed.

	ZeroMemory(szBuf,sizeof(szBuf));
	snr = SNPS3GetXMBSettings(m_targetId,szBuf,&uSize,m_bSync);

	if (SUCCEEDED(snr))
	{
		// Don't be too verbose, the caller might want to parse the output string!
		std::cout << szBuf << std::endl;
	}
	else
	{
		PrintError(snr, L"Error retrieving XMB settings!\n");
		return false;
	}
	return true;
}

bool XMBCommand::DoSet()
{
	SNRESULT snr;
	
	snr = SNPS3SetXMBSettings(m_targetId,m_strSetArgs.c_str(),m_bReset);
	if (SUCCEEDED(snr))
	{
	}
	else
	{
		PrintError(snr, L"Error setting XMB settings!\n");
		return false;
	}
	return true;
}

void XMBCommand::DisplayUsageHelp() const
{
	std::cout << "The format command allows you to format a target's hard drive." << std::endl << std::endl;

	std::cout << "Usage: PS3Ctrl xmb <options>" << std::endl << std::endl;
	std::cout << "  Where <options> are the following:" << std::endl << std::endl;
	std::cout << "  -get" << "\t\t" << "Display the XMB settings of the target" << std::endl;
	std::cout << "  -sync" << "\t\t" << "Retrieve XMB settings directly from the target (with -get)" << std::endl;
	std::cout << "  -set <string>" << "\t" << "Sets the XMB settings" << std::endl;
	std::cout << "  -reset" << "\t" << "Reset the target after setting the XMB settings (with -set)" << std::endl;
	std::cout << std::endl;

	DisplayCommonOptions();
}
