/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_ARG_OPTION_H
#define SINGLE_ARG_OPTION_H

#pragma once

#include "Argument.h"
#include "Exceptions.h"

namespace commandargutils
{
	template <class T>
	class SingleArgOption : public Argument
	{
	public:
						SingleArgOption(std::string optionName
							, std::string longName
							, T initialValue
							, bool bRequired = false
							, bool bValueRequired = true
							, UINT32 taskId = NO_TASK
							);
		virtual			~SingleArgOption();
		virtual bool	ProcessArg(UINT32 &i, std::vector<std::string> &args);

		T				GetValue() 
		{
			return m_val;
		}
	private:
		T				m_val;
		bool			m_bValueRequired;
	};

	template <class T>
	bool commandargutils::SingleArgOption<T>::ProcessArg(UINT32 &i, std::vector<std::string> &args)
	{
		_ASSERT(i < args.size());
		
		std::string &flag = args[i];

		if (IsMatch(flag))
		{
			if ((++i) >= args.size())
			{
				if (m_bValueRequired)
					throw ArgumentException(std::string("Argument '") + m_longName + std::string("' requires a value to be passed!"));
				else
				{
					--i;
					m_bPassed = true;
					return true;
				}
			}
			else if (!IsNotFlag(args[i]))
			{
				// We don't have a value for this switch
				if (m_bValueRequired)
					throw ArgumentException("Argument value not passed!");
				else
				{
					--i;
					m_bPassed = true;
					return true; // Don't read value. Leave default
				}
			}
			
			std::string &value = args[i];

			// Extract Value
			ConvertValue(m_val, value, typename ArgTraits<T>::TypeClass());
			m_bPassed = true;
		}
		else
			return false;

		return true;
	}

	// Implementation
	template <class T>
	commandargutils::SingleArgOption<T>::SingleArgOption(std::string optionName 
														, std::string longName
														, T initialValue
														, bool bRequired
														, bool bValueRequired
														, UINT32 taskId
														)
	: Argument(optionName, longName, bRequired, taskId)
	, m_val(initialValue)
	, m_bValueRequired(bValueRequired)
	{

	}

	template <class T>
	commandargutils::SingleArgOption<T>::~SingleArgOption()
	{

	}

}; // end of name space
#endif