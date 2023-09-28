/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <vector>
#include <algorithm>
#include "Exceptions.h"
#include "Argument.h"

namespace commandargutils
{
	typedef std::vector<Argument*>::const_iterator ArgumentConstIterator;
	typedef std::vector<Argument*>::iterator ArgumentIterator;
	typedef std::vector<std::string>::const_iterator StringConstIterator;
	typedef std::vector<std::string>::iterator StringIterator;
	typedef std::vector<std::string> StringVector;

	bool ArgumentSortPredicate(const Argument* arg1, const Argument* arg2);

	class CommandLineHandler
	{
	public:
									CommandLineHandler();
		virtual						~CommandLineHandler();
		void						AddArgument(Argument& arg);
		void						AddArgument(Argument* pArg);
		void						Parse(std::vector<std::string>& argumentsVect);
		void						Reset();
		std::vector<Argument*>		&GetArguments();
		std::vector<std::string>	&GetRemainingArguments();
		UINT32						GetLastFlagPositionIndex() const;

	private:
		void						SortArguments();
		
		std::vector<Argument*>		m_arguments;
		std::vector<std::string>	m_remainingArgs;
		int							m_requiredArgCount;
		size_t						m_lastFlagPositionIndex;
	};
} // End of namespace
#endif