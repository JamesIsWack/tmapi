/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef POWER_COMMAND_H
#define POWER_COMMAND_H

#include "TargetCommand.h"
#include "StandardOption.h"

class PowerCommand : public TargetCommand
{
public:
	enum Task 
	{ 
		POWER_ON = 0,
		POWER_OFF = 1,
		FORCE_POWER_OFF = 2,
	};

					PowerCommand();
	virtual			~PowerCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			PerformPowerOff(bool bForcePowerOff = false);
	bool			PerformPowerOn();
	virtual void	DisplayUsageHelp() const;

	Task			m_task;
};

TargetCommand* PowerCommandFactory(void);

#endif