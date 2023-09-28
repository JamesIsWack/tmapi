/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef CMDLINE_EXCEPTIONS_H
#define CMDLINE_EXCEPTIONS_H

#include <exception>

namespace commandargutils
{
	class CommandLineHandlerException : public std::exception
	{
	public:
		CommandLineHandlerException(const char *exceptionStr)
			: std::exception()
			, m_exception(exceptionStr) { }
		CommandLineHandlerException(std::string& exceptionStr)
			: std::exception()
			, m_exception(exceptionStr) { }
		virtual ~CommandLineHandlerException() throw() { }

		virtual const char* what() const throw()
		{
			return m_exception.c_str();
		}
	private:
		std::string m_exception;
	};

	class ArgumentException : public std::exception
	{
	public:
		ArgumentException(const char *exceptionStr)
			: std::exception()
			, m_exception(exceptionStr) { }
		ArgumentException(std::string& exceptionStr)
			: std::exception()
			, m_exception(exceptionStr) { }
		virtual ~ArgumentException() throw() { }

		virtual const char* what() const throw()
		{
			return m_exception.c_str();
		}
	private:
		std::string m_exception;
	};

}; // end of namespace
#endif
