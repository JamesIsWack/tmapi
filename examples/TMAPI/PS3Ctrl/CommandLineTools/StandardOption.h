/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef STANDARD_OPTION_H
#define STANDARD_OPTION_H

#pragma once

#include "Argument.h"
#include "Exceptions.h"

namespace commandargutils
{
	class StandardOption : public Argument
	{
	public:
						StandardOption(std::string optionName
							, std::string longName
							, bool initiallySet = false
							, bool bRequired = false
							, UINT32 taskId = NO_TASK);
		virtual			~StandardOption();

		virtual bool	ProcessArg(UINT32 &i, std::vector<std::string> &args);

		bool			IsSet() const 
		{ 
			return m_bSet; 
		}

		virtual void	SetPassed() 
		{
			Argument::SetPassed();
			m_bSet = true;
		}
	private:
		bool			m_bSet;
	};
} // end of namespace
#endif