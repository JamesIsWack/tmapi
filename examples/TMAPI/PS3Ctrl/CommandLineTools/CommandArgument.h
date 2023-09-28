/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_ARGUMENT_H
#define COMMAND_ARGUMENT_H

#include "Argument.h"
#include "Exceptions.h"

namespace commandargutils
{
	class CommandArgument : public Argument
	{
	public:
						CommandArgument(std::string commandName);
		virtual			~CommandArgument();	
		virtual bool	ProcessArg(UINT32 &i, std::vector<std::string> &args);
		bool			IsSet() const;
		virtual void	SetPassed();

	protected:
		virtual bool	IsMatch(const std::string& arg) const;
	private:
		bool			m_bSet;
	};
} // end of namespace
#endif