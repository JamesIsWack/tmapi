/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef MULTI_ARG_OPTION_H
#define MULTI_ARG_OPTION_H

#pragma once

#include <vector>
#include "Argument.h"
#include "Exceptions.h"

namespace commandargutils
{
	template <class T>
	class MultiArgOption : public Argument
	{
	public:
						MultiArgOption(std::string optionName
						, std::string longName
						, bool bRequired = false
						, int requiredValueCount = -1
						, UINT32 taskId = NO_TASK);
		virtual			~MultiArgOption();
		virtual bool	ProcessArg(UINT32 &i, std::vector<std::string> &args);

		std::vector<T>	GetValues()
		{ 
			return m_values; 
		}

		void SetValidValues(std::vector<T> &validValues)
		{
			m_validValues = validValues;
		}

	protected:
		std::vector<T>		m_values;
		std::vector<T>		m_validValues;
		UINT32				m_requiredValueCount;
	};

	template <class T>
	commandargutils::MultiArgOption<T>::MultiArgOption(std::string optionName
														, std::string longName 
														, bool bRequired
														, int requiredValueCount/* = -1*/
														, UINT32 taskId
														)
	: Argument(optionName, longName, bRequired, taskId)
	, m_requiredValueCount(requiredValueCount)
	{

	}

	template <class T>
	commandargutils::MultiArgOption<T>::~MultiArgOption()
	{

	}

	template <class T>
	bool commandargutils::MultiArgOption<T>::ProcessArg(UINT32 &i, std::vector<std::string> &args)
	{
		_ASSERT(static_cast<UINT32>(i) < args.size());

		std::string &flag = args[i];

		if (IsMatch(flag))
		{
			++i;
			while(i < args.size() && IsNotFlag(args[i]))
			{
				std::string &value = args[i];
				T dstValue;
				
				try
				{
					ConvertValue(dstValue, value, typename ArgTraits<T>::TypeClass());
				}
				catch (...)
				{
					// Ignore exception since we prob just hit the next argument that was a different type
					break;
				}

				if (m_validValues.size() > 0 && std::find(m_validValues.begin(), m_validValues.end(), dstValue) == m_validValues.end())
					break;

				m_values.push_back(dstValue);
				++i;
			}

			--i;

			if (m_requiredValueCount != -1 && m_requiredValueCount != m_values.size())
				throw ArgumentException(std::string("Incorrect number of values for argument ") + m_longName);
			
			m_bPassed = true;

		}
		else
			return false;

		return true;
	}
} // end of namespace
#endif