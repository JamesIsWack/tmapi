/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef DEFINES_H
#define DEFINES_H

// Exit codes
#define PS3CTRL_EXIT_OK                (0)
#define PS3CTRL_EXIT_ERROR             (-1)

// Helper macros
#define IS_VERBOSE  (!SurpressErrorLogging())
#define ESCAPE_KEY	(27)

#define INVALID_TARGET				  (0xffffffff)
#define INVALID_PROCESS				  (0xffffffff)
#define NO_TASK						  (0xffffffff)
enum MSG_LEVEL { ML_WARN, ML_INFO, ML_ERROR };

#define DEFAULT_TARGET_TYPE "PS3_DEH_TCP"

#define PS3FLASH_DEFAULT_UPDATER "%SCE_PS3_ROOT%\\target\\images\\PS3ToolUpdater.self"
#define PS3FLASH_DEFAULT_PUP "%SCE_PS3_ROOT%\\target\\updater\\ref-tool\\PS3UPDAT.PUP"

#endif