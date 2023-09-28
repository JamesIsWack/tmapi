/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2012, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef FILESYSTEM_COMMAND_H
#define FILESYSTEM_COMMAND_H

#include "TargetCommand.h"
#include "SingleArgOption.h"

class MapFileSystemCommand : public TargetCommand
{
public:
					MapFileSystemCommand();
	virtual			~MapFileSystemCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoMapFileSystem();
	virtual void	DisplayUsageHelp() const;

	char			m_driveLetter;
	bool			m_bArgsOk;
};

class UnmapFileSystemCommand : public TargetCommand
{
public:
					UnmapFileSystemCommand();
	virtual			~UnmapFileSystemCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();

protected:
	bool			DoUnmapFileSystem();
	virtual void	DisplayUsageHelp() const;
	bool			m_bArgsOk;
};

TargetCommand* MapFileSystemCommandFactory(void);
TargetCommand* UnmapFileSystemCommandFactory(void);

#endif // FILESYSTEM_COMMAND_H