/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef SET_FLAGS_COMMAND_H
#define SET_FLAGS_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class SetFlagsCommand : public TargetCommand
{
public:
	enum Task 
	{	
		SET_CORE_DUMP_FLAGS, 
		SET_SCREEN_CAPTURE_FLAGS,
		ENABLE_SCREEN_CAPTURE,
		SET_EXTRA_LOAD_FLAGS
	};

					SetFlagsCommand();
	virtual			~SetFlagsCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool				SetScreenCaptureFlags();
	bool				EnableScreenCapture();
	bool				SetCoreDumpFlags();
	bool				SetExtraLoadFlags();
	virtual void		DisplayUsageHelp() const;

	UINT64				m_coreDumpFlags;
	UINT64				m_screenCaptureFlags;
	UINT64				m_extraLoadFlags;
	UINT64				m_extraLoadFlagsMask;
	std::vector<Task>	m_tasks;
};

TargetCommand* SetFlagsCommandFactory(void);

#endif