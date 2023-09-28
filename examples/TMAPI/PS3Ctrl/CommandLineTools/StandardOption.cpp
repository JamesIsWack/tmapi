/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "StandardOption.h"

using namespace commandargutils;

StandardOption::StandardOption(std::string optionName
					, std::string longName 
					, bool initiallySet
					, bool bRequired
					, UINT32 taskId)
					: Argument(optionName, longName, bRequired, taskId)
					, m_bSet(initiallySet)
{

}

StandardOption::~StandardOption()
{

}

bool StandardOption::ProcessArg(UINT32 &i, std::vector<std::string> &args)
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