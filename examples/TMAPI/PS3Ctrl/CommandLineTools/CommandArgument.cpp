/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "CommandArgument.h"

using namespace commandargutils;

CommandArgument::CommandArgument(std::string commandName)
								  : Argument(commandName, commandName, false, NO_TASK)
								  , m_bSet(false)
{

}

CommandArgument::~CommandArgument()
{

}

bool CommandArgument::ProcessArg(UINT32 &i, std::vector<std::string> &args)
{
	_ASSERT(static_cast<UINT32>(i) < args.size());

	std::string &flag = args[i];

	if (IsMatch(flag))
	{
		m_bSet = true;
		m_bPassed = true;
	}
	else
		return false;

	return true; 
}

bool CommandArgument::IsMatch(const std::string& arg) const
{
	if (arg == m_shortName || arg == m_longName)
		return true;

	return false;
}

bool CommandArgument::IsSet() const
{ 
	return m_bSet; 
}

void CommandArgument::SetPassed()
{
	Argument::SetPassed();
	m_bSet = true;
}