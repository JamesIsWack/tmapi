/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include "CommandLineHandler.h"

using namespace commandargutils;

bool commandargutils::ArgumentSortPredicate(const Argument* arg1, const Argument* arg2)
{
	return arg1->GetSortValue() < arg2->GetSortValue();
}

CommandLineHandler::CommandLineHandler()
: m_requiredArgCount(0)
, m_lastFlagPositionIndex(0)
{

}

CommandLineHandler::~CommandLineHandler()
{

}

void CommandLineHandler::Parse(std::vector<std::string>& argumentsVect)
{
	UINT32 sortVal = 0;

	if (argumentsVect.size() < 1)
		return;

	int requiredCount = 0;

	for (UINT i = 0; i < argumentsVect.size(); ++i)
	{
		ArgumentConstIterator it2 = m_arguments.begin();
		for (; it2 != m_arguments.end(); ++it2)
		{
			Argument *pArg = *it2;
			if (pArg->ProcessArg(i, argumentsVect))
			{	
				if (i > m_lastFlagPositionIndex)
					m_lastFlagPositionIndex = i;

				pArg->SetSortValue(sortVal++);
				if (pArg->IsRequired() && pArg->GetParentDependency() == NULL)
					++requiredCount;

				break;
			}
		}
	}

	for (size_t i = m_lastFlagPositionIndex + 1; i < argumentsVect.size(); ++i)
	{
		m_remainingArgs.push_back(argumentsVect[i]);
	}
	// Check dependencies
	ArgumentConstIterator it = m_arguments.begin();
	for (; it != m_arguments.end(); ++it)
	{
		Argument* pParentDependency = (*it)->GetParentDependency();

		if (pParentDependency)
		{
			if (pParentDependency->IsPassed() && (*it)->IsRequired() && !(*it)->IsPassed())
				throw ArgumentException(std::string("Argument: '") + (*it)->GetLongName()+ std::string("' is required when using argument: '") + pParentDependency->GetLongName()+ std::string("'"));
			else if ((*it)->IsPassed() && !pParentDependency->IsPassed())
				throw ArgumentException(std::string("Argument: '") + (*it)->GetLongName()+ std::string("' is dependent on argument: '") + pParentDependency->GetLongName()+ std::string("'"));
		}

		if ((*it)->IsPassed())
		{
			Argument* pImpliedDependency = (*it)->GetImpliedArgument();
			if (pImpliedDependency && !pImpliedDependency->IsPassed())
			{
				pImpliedDependency->SetPassed();
				if (pImpliedDependency->IsRequired())
					++requiredCount;
			}
		}

	}

	if (requiredCount != m_requiredArgCount)
		throw ArgumentException("Not all arguments were passed!");

	SortArguments();
}

void CommandLineHandler::AddArgument(Argument& arg)
{
	AddArgument(&arg);
	if (arg.IsRequired() && arg.GetParentDependency() == NULL)
		++m_requiredArgCount;
}

void CommandLineHandler::AddArgument(Argument* pArg)
{
	m_arguments.push_back(pArg);
}

void CommandLineHandler::Reset()
{
	m_arguments.clear();
	m_remainingArgs.clear();
}

void CommandLineHandler::SortArguments()
{
	std::sort(m_arguments.begin(), m_arguments.end(), ArgumentSortPredicate);
}

std::vector<Argument*>& CommandLineHandler::GetArguments()
{ 
	return m_arguments; 
}
std::vector<std::string>& CommandLineHandler::GetRemainingArguments()
{ 
	return m_remainingArgs; 
}

UINT32 commandargutils::CommandLineHandler::GetLastFlagPositionIndex() const
{
	return static_cast<UINT32>(m_lastFlagPositionIndex);
}
