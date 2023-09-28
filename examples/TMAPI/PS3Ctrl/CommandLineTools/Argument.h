/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>
#include <sstream>
#include <vector>
#include "ArgumentTraits.h"
#include "Defines.h"

namespace commandargutils
{
	class Argument
	{
	public:
		Argument(std::string shortName
				, std::string longName
				, bool bRequired = false
				, UINT32 taskId = NO_TASK)
				: m_shortName(shortName)
				, m_longName(longName)
				, m_bRequired(bRequired)
				, m_bPassed(false)
				, m_pParentDependency(NULL)
				, m_pImpliedArgument(NULL)
				, m_sortVal(0)
				, m_taskId(taskId)
		{

		}

		virtual ~Argument()
		{

		}

		std::string& GetShortName()
		{ 
			return m_shortName; 
		}

		std::string& GetLongName()
		{ 
			return m_longName; 
		}

		bool IsRequired() const 
		{ 
			return m_bRequired; 
		}

		virtual bool ProcessArg(UINT32 &i, std::vector<std::string> &args) = 0;
		virtual bool IsMatch(const std::string& arg) const
		{
			if (arg == std::string("-")+m_shortName || arg == std::string("--")+m_longName)
				return true;

			return false;
		}
		virtual bool IsPassed() const { return m_bPassed; }
		virtual void SetPassed() { m_bPassed = true; }

		void SetParentDependency(Argument *pArgument)
		{
			m_pParentDependency = pArgument;
		}
		Argument *GetParentDependency() const
		{ 
			return m_pParentDependency; 
		}
		
		void SetImpliedArgument(Argument *pArgument)
		{
			m_pImpliedArgument = pArgument;
		}
		
		Argument *GetImpliedArgument() const
		{ 
			return m_pImpliedArgument; 
		}
		
		void SetSortValue(UINT32 val) 
		{ 
			m_sortVal = val; 
		}

		UINT32 GetSortValue() const 
		{ 
			return m_sortVal; 
		}

		UINT32 GetTaskId() const 
		{ 
			return m_taskId; 
		}

		void SetTaskId(UINT32 taskId) 
		{ 
			m_taskId = taskId; 
		}

		static bool IsNotFlag(const std::string& arg)
		{
			return !(arg.substr(0, 1) == "-" || arg.substr(0, 2) == "--");
		}

	protected:

		std::string m_shortName;
		std::string m_longName;
		bool		m_bRequired;
		bool		m_bPassed;
		Argument	*m_pParentDependency;
		Argument	*m_pImpliedArgument;
		UINT32		m_sortVal;
		UINT32		m_taskId;
	};

	// Template functions
	template <typename T> bool ReadInt(std::string &numStr, T& value)
	{
		bool	negative = false;
		T		result = 0;

		const char *ptr = numStr.c_str();

		if (*ptr == '-')
		{
			negative = true;
			ptr++;
		}

		if (*ptr == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
		{
			ptr += 2;

			for (;;)
			{
				if (*ptr >= '0' && *ptr <= '9')
					result = (result << 4) | (*ptr++ - '0');
				else if (*ptr >= 'A' && *ptr <= 'F')
					result = (result << 4) | (*ptr++ - 'A' + 10);
				else if (*ptr >= 'a' && *ptr <= 'f')
					result = (result << 4) | (*ptr++ - 'a' + 10);
				else
					break;
			}
		}
		else
		{
			if (*ptr >= '0' && *ptr <= '9')
			{
				while (*ptr >= '0' && *ptr <= '9')
					result = result * 10 + (*ptr++ - '0');
			}
			else
				return 0;
		}

		if (negative)
			value = 0 - result;
		else
			value = result;

		return true;
	}

	template<typename T> 
	void ConvertValue(T &dstVal, const std::string& value, PrimitiveType)
	{
		std::stringstream is(value);

		UINT32 valuesAdded = 0;

		for (; is.good(); ++valuesAdded)
		{
			is >> dstVal;
		}

		if (is.fail() || valuesAdded < 1 || valuesAdded > 1)
			throw ArgumentException("Could not parse argument");
	}

	template<typename T>
	void ConvertValue(T &dstVal, const std::string& value, StringType)
	{
		dstVal = value;
	}

	template<typename T>
	void ConvertValue(T &dstVal, std::string& value, IntegerType)
	{
		if (!ReadInt(value, dstVal))
			throw ArgumentException("Could not parse value");
	}
} // End of namespace

#endif