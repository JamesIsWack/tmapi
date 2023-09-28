/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef SYNC_COMMAND_H
#define SYNC_COMMAND_H

#include <queue>
#include "TargetCommand.h"

class SyncCommand : public TargetCommand
{
public:
	enum TransferType { 
		TX_TYPE_UNKNOWN,
		TX_TYPE_DIRECTORY_TO_DIRECTORY,
		TX_TYPE_FILE_TO_DIRECTORY,
		TX_TYPE_FILE_TO_FILE
	};

	// Direction from the PC's perspective
	enum TransferDirection {
		TX_DIRECTION_UPLOAD = 0,
		TX_DIRECTION_DOWNLOAD
	};
					SyncCommand();
	virtual			~SyncCommand();
	virtual bool	ParseArgs(std::vector<std::string>& arguments);
	virtual int		Run();
	void			ProcessTransferEvent(HTARGET hTarget, UINT uLength, BYTE* pData);

protected:
	std::string				GetFileNameWithExt(const char* szPath);

	bool					PerformFileToFile();
	bool					PerformDirectoryToDirectory();
	bool					PerformFileToDirectory();
	bool					SetupFileEvents();
	static void __stdcall	TransferEventCallback(HTARGET hTarget, UINT uEventType, UINT uEventParam, SNRESULT snr,
		UINT uLength, BYTE* pEventData, void* pUserData);
	TransferType			IdentifyTransferType();
	void					CompleteTransfer(UINT id);
	bool					ProcessEvents();
	SNRESULT				ClearPendingMessages(void);
	virtual void			DisplayUsageHelp() const;

	std::vector<std::string>	m_srcFiles;
	std::string					m_dstPath;
	bool						m_bForceSync;
	bool						m_bWaitForTransfers;
	std::deque<UINT32>			m_PendingTransfers;
	TransferDirection			m_Direction;
};

TargetCommand* SyncCommandFactory(void);

#endif