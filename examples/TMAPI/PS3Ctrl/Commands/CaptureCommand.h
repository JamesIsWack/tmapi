/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef CAPTURE_COMMAND_H
#define CAPTURE_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class CaptureCommand : public TargetCommand
{
public:
					CaptureCommand();
	virtual			~CaptureCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoVramCapture();
	virtual void	DisplayUsageHelp() const;
	bool			AutoGetProcessId(UINT32 &processId) const;

	std::string		m_vramOutputPath;
	UINT32			m_processId;
};

TargetCommand* CaptureCommandFactory(void);

#endif