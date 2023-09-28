/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef BD_COMMAND_H
#define BD_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class BDCommand : public TargetCommand
{
public:
					BDCommand();
	virtual			~BDCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	typedef enum	{BDCMD_NONE=-1,BDCMD_COPY=0,BDCMD_MOUNT,BDCMD_UNMOUNT,BDCMD_FORMAT,BDCMD_QUICKFORMAT,BDCMD_QUERY} bdcmd_t;

	bool			DoFormat(UINT32 mode);
	bool			DoCopy();
	bool			DoMount();
	bool			DoUnmount();
	bool			DoQuery();
	virtual void	DisplayUsageHelp() const;

	bdcmd_t			m_Command;
	bool			m_bWait;
	volatile bool	m_bFinished;
	SNRESULT		m_Result;
	UINT32			m_uXferid;
		
	

	std::string			m_strDevice;
	std::string			m_strFile;
	static void __stdcall TargetEventCallback(HTARGET target,UINT32 uType,UINT32 uEventSpecific,SNRESULT resultCode,UINT32 udataLength,BYTE* data ,void* pUserdata);		
};

TargetCommand* BDCommandFactory(void);

#endif