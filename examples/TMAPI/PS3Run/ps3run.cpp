/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <algorithm>
#include <conio.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <time.h>
#include <vector>

#include "tmver.h"
#include "PS3tmapi.h"

//////////////////////////////////////////////////////////////////////////////
///  DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

// Command types
#define PS3RUN_CMD_FORCE_DISCONNECT     (0x00000004)
#define PS3RUN_CMD_RESET                (0x00000008)
#define PS3RUN_CMD_GET_TTY              (0x00004000)
#define PS3RUN_CMD_SET_FS_DIR           (0x00010000)
#define PS3RUN_CMD_SET_HOME_DIR         (0x00020000)
#define PS3RUN_CMD_WAIT_ELF_EXIT        (0x00040000)
#define PS3RUN_CMD_SEND_TTY             (0x00080000)
#define PS3RUN_CMD_CHECK_FOR_ABORT      (0x00100000)
#define PS3RUN_CMD_CHECK_REL_TIMEOUT    (0x00200000)
#define PS3RUN_CMD_CHECK_ABS_TIMEOUT    (0x00400000)
#define PS3RUN_CMD_RESTORE_BOOT_PARAMS  (0x00800000)
#define PS3RUN_CMD_POWER_CYCLE			(0x01000000)
#define PS3RUN_CMD_CHECK_SDK_BF_RUN		(0x02000000)
#define PS3RUN_CMD_ADD_TARGET			(0x04000000)
#define PS3RUN_CMD_DEL_TARGET			(0x08000000)

// Global options
#define PS3RUN_OPT_QUIET              (0x00000001)
#define PS3RUN_OPT_DISCONNECT         (0x00000002)
#define PS3RUN_OPT_ALWAYS_DISCONNECT  (0x00000004)
#define PS3RUN_OPT_SEND_TTY_FROM_FILE (0x00000010)
#define PS3RUN_OPT_LOG_TO_FILE		  (0x00000020)

// Exit codes
#define PS3RUN_EXIT_OK                (0)
#define PS3RUN_EXIT_ERROR             (-1)

// Helper macros
#define IS_VERBOSE  !(g_TargetOpt.nOptFlags & PS3RUN_OPT_QUIET)
#define ESCAPE_KEY	(27)

#define MAX_LINE_SIZE			        (1024)

//////////////////////////////////////////////////////////////////////////////
///  STRUCTURES
//////////////////////////////////////////////////////////////////////////////

struct TGT_OPTIONS
{
	HTARGET  hTargetId;
    std::wstring  wcsTgtName;

	WCHAR     szElfFileName[MAX_PATH];
	WCHAR*    pszBinFileName;

	UINT32	uProcessId;
	UINT64	uThreadId;

	int		 argc;
	WCHAR**   argv;

	WCHAR*    pszFSDir;
	WCHAR*    pszHomeDir;

	UINT64   nResetParam;
	UINT64   nBootParam;
	UINT64   nBootMask;

	UINT     nBinaryAddr;

	UINT     nCmdFlags;
	UINT     nOptFlags;

	std::vector<WCHAR*> arrTTYChannelNames;
	std::vector<UINT> arrTTYChannels;

	WCHAR*    pszTTYOutChannel;
	UINT     nTTYOutChannel;

	FILE*    pTTYImportSource;

	WCHAR*    pszAbortString;
	int      nAbortExitCode;

	UINT64   uTimeoutDuration;
	UINT64   uAbsTimeoutDuration;

	UINT32	nModulePriority;
	UINT32	nDebugFlags;

	std::string	strTargetType;
	std::string strConnectionParameters;

	std::string strTargetAdd;
	std::string strTargetDel;

	std::string paramSfoDir;

	std::wstring strLogFileName;
};

struct RESET_PARAMETERS
{
	struct Parameter
	{
		UINT64	uValue;
		UINT64	uMask;
	};

	struct Parameter Reset;
	struct Parameter Boot;
	struct Parameter System;
};

static int Parse_a(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_b(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_c(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_d(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_e(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_f(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_g(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_h(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_i(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_k(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_l(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_m(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_n(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_p(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_q(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_r(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_s(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_t(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_u(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_v(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_w(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_y(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_z(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);

static int Parse_add(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_del(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_pri(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_dbg(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_con(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);
static int Parse_sfo(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);

typedef int (*LPFNPARSEPROC)(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv);

struct CMD_LINE_PARAM
{
	bool			bDoubleDash;
	std::wstring	word;
	LPFNPARSEPROC	proc;
	std::wstring	description;
};

CMD_LINE_PARAM	gCmdLineParam[] = 
{
	{false, L"t"		, Parse_t  , L"    %s <target>         - Specify target (default target if not specified)"},
	{false, L"a"		, Parse_a  , L"    %s <text>           - Abort if <text> is seen in the TTY stream"},
	{true , L"add"		, Parse_add, L"    %s <type>,<target>  - Add a new target. <type> is the target type, between:\n"
	                                 L"                         PS3_DEH_TCP for a DECR-1000 (Reference Tool)\n"
									 L"                         PS3_512_DBG_DEX for a DECR-1400 (Reference tool)\n"
									 L"                         PS3_DBG_DEX for a DECHA00/DECHJ00 (Debugging Station)\n"
									 L"                         PS3_CORE_DUMP for a Core Dump target"},

	{true , L"conn"		, Parse_con, L"    %s <ip[:port]> - Specify connection parameter"},
	{true , L"delete"	, Parse_del, L"    %s <target>  - Delete a target"},
	{true , L"pri"		, Parse_pri, L"    %s <priority>   - Set priority of module to be loaded"},
	{true , L"debug"	, Parse_dbg, L"    %s            - Enable module debugging"},
	{true, L"sfodir"	, Parse_sfo, L"    %s <dir>	- Use param.sfo in <directory> when loading ELF. To use the ELF directory, specify its full path here."},

	{false, L"e"		, Parse_e  , L"    %s <text>           - Exit if <text> is seen in the TTY stream"},
	{false, L"c"		, Parse_c  , L"    %s <channel>        - Send keystrokes (or -i<file>) to specified TTY channel"},
	{false, L"i"		, Parse_i  , L"    %s <file>           - Import TTY text from file ('-' for stdin)"},
	{false, L"d"		, Parse_d  , L"    %s                 - Disconnect from target (if not previously connected)\n"
									 L"    -da                - Always disconnect from target when done"},
	{false, L"n"		, Parse_n  , L"    %sd                - No disconnect (default)\n"
									 L"    -nr                - No reset (default)"},
	{false, L"k"		, Parse_k  , L"    %s                 - Force disconnect another user"},
	{false, L"r"		, Parse_r  , L"    %s <mode>           - Reset target first (defaults to soft reset)\n"
									 L"                         -r0=soft, -r1=hard, -r2=quick"},
	{false, L"s"		, Parse_s  , L"    %s                 - Perform power off then power on if reset fails"},
	{false, L"b"		, Parse_b  , L"    %s <hex>            - Use <hex> as boot parameter for reset"},
	{false, L"m"		, Parse_m  , L"    %s <hex>            - Use <hex> as boot parameter mask for reset"},
	{false, L"f"		, Parse_f  , L"    %s <path>           - Set file serving directory"},
	{false, L"h"		, Parse_h  , L"    %s <path>           - Set home directory"},
	{false, L"p"		, Parse_p  , L"    %s<channel>        - Print output on a TTY channel (or empty for all)"},
	{false, L"u"		, Parse_u  , L"    %s                 - Restore boot parameters (if changed) to their original\n"
									 L"                         value"},
	{false, L"w"		, Parse_w  , L"    %s                 - Wait for process exit.  PS3Run exits on receipt"},
	{false, L"q"		, Parse_q  , L"    %s                 - Operate quietly if no errors"},
	{false, L"y"		, Parse_y  , L"    %s <timeout>        - Terminate after <timeout> seconds"},
	{false, L"z"		, Parse_z  , L"    %s <timeout>        - Terminate run after <timeout> seconds without event"},
	{false, L"v"		, Parse_v  , L"    %s                 - API and target SDK must match before ELF file will run"},
	{false, L"l"		, Parse_l  , L"    %s <file>           - Log the output to a file"},

	// End marker
	{false, L"~"		, NULL	   , L""}
};

//////////////////////////////////////////////////////////////////////////////
///  GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////

static std::vector<SNPS3TargetInfo*>	g_Targets;
static TGT_OPTIONS						g_TargetOpt = {0};
static TTYSTREAM						*g_pTTYStreams = NULL;
static UINT								g_nNumTTYStreams = 0;
static bool								g_bQuit = false;
static int								g_nExitCode = PS3RUN_EXIT_OK;
static std::wstring						g_TTYBuffer;
static __time64_t						g_TimeoutTime = 0;
static __time64_t						g_AbsoluteTimeoutTime = 0;
static bool								g_bAbortTextFound = false;

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Usage
///  @brief     Display program usage information.
//////////////////////////////////////////////////////////////////////////////

static void Usage(void)
{
	puts("PS3RUN v" STRINGIFY(TM_VERSION_DOTS) " ("__DATE__") - PS3TM Command Line Tool");
	puts(STR_PRODG_COPYRIGHT);

	puts("Usage: PS3run <options> [<file> [<args>]]\n");
	puts("    Where <options> is one of the following:");

	WCHAR szFormattedMsg[4 * 1024];
	std::wstring	param;

	int i = 0;
	while (gCmdLineParam[i].proc)
	{
		param = gCmdLineParam[i].bDoubleDash ? L"--" : L"-";
		param += gCmdLineParam[i].word;

		swprintf_s(szFormattedMsg, _countof(szFormattedMsg)-1, gCmdLineParam[i].description.c_str(), param.c_str());
		_putws(szFormattedMsg);
		i++;
	}
	puts("");
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    PrintMessage
///  @brief     Prints a message (with var args)
//////////////////////////////////////////////////////////////////////////////
enum MSG_LEVEL { ML_WARN, ML_INFO, ML_ERROR };

static void PrintMessage(MSG_LEVEL nMsgLevel, const WCHAR *pszFormat, ...)
{
	if (nMsgLevel == ML_INFO && !IS_VERBOSE)
		return;

	std::wstring Message;
	if (nMsgLevel == ML_WARN)
		Message = L"Warning - ";
	else if (nMsgLevel == ML_INFO)
		Message = L"Info - ";
	else
		Message = L"Error - ";

	va_list	args;
	va_start(args, pszFormat);

	WCHAR szBuffer[4 * 1024];

	_vsnwprintf(szBuffer, _countof(szBuffer), pszFormat, args);

	va_end(args);

	if (!g_TargetOpt.wcsTgtName.empty())
	{
		Message += L"[";
		Message += g_TargetOpt.wcsTgtName;
		Message += L"] ";
	}

	Message += szBuffer;

	_putws(Message.c_str());

	if (g_TargetOpt.nOptFlags&PS3RUN_OPT_LOG_TO_FILE && !g_TargetOpt.strLogFileName.empty())
	{
		FILE* f = _wfopen(g_TargetOpt.strLogFileName.c_str(), L"a");
		if (f)
		{
			fwprintf(f, Message.c_str());
			fclose(f);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
///  @anchor    PrintError
///  @brief     Prints an error message along with the translated SNRESULT.
//////////////////////////////////////////////////////////////////////////////

static void PrintError(SNRESULT snr, const WCHAR* pszMessage, ...)
{
	const char* pszError = NULL;
	SNPS3TranslateError(snr, &pszError);
	if (pszError == NULL)
		pszError = "Unknown";

	va_list	args;
	va_start(args, pszMessage);

	WCHAR szFormattedMsg[4 * 1024];
	_vsnwprintf(szFormattedMsg, _countof(szFormattedMsg), pszMessage, args);
	szFormattedMsg[_countof(szFormattedMsg)-1] = '\0';

	PrintMessage(ML_ERROR, L"%s: %ld (%s)\n", szFormattedMsg, snr, CUTF8ToWChar(pszError).c_str(), args);

	UINT32 qualifier = 0;
	char* szErrorQual = NULL;
	if (SN_SUCCEEDED(SNPS3GetErrorQualifier(&qualifier, &szErrorQual)))
	{
		PrintMessage(ML_ERROR, L"Extra info: 0x%08X - %s\n", qualifier, szErrorQual ? CUTF8ToWChar(szErrorQual).c_str() : L"Unknown");
	}

	va_end(args);

}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    UpdateTimeout
///  @brief     Update the timeout time.
//////////////////////////////////////////////////////////////////////////////

static void UpdateTimeout()
{
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_CHECK_REL_TIMEOUT)
	{
		__time64_t uNow;
		_time64(&uNow);

		// Comment out this line if you want an absolute timeout
		// rather than relative to the last event.
		g_TimeoutTime = uNow + g_TargetOpt.uTimeoutDuration;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    CheckTimeout
///  @brief     Check to see if we've timed out.
//////////////////////////////////////////////////////////////////////////////

static void CheckTimeout()
{
	if (g_bQuit)
		return;

	if (g_TargetOpt.nCmdFlags & (PS3RUN_CMD_CHECK_REL_TIMEOUT|PS3RUN_CMD_CHECK_ABS_TIMEOUT))
	{
		__time64_t uNow;
		_time64(&uNow);

		if ((g_TargetOpt.nCmdFlags&PS3RUN_CMD_CHECK_REL_TIMEOUT) && uNow > g_TimeoutTime)
		{
			g_bQuit = true;
			g_nExitCode = PS3RUN_EXIT_ERROR;
		}

		if ((g_TargetOpt.nCmdFlags&PS3RUN_CMD_CHECK_ABS_TIMEOUT) && uNow > g_AbsoluteTimeoutTime)
		{
			g_bQuit = true;
			g_nExitCode = PS3RUN_EXIT_ERROR;
		}

		if (g_bQuit)
		{
			PrintMessage(ML_WARN, L"Timeout occurred\n");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    FinalizeTTYStreams
///  @brief     Cleanup the global TTYSTREAM data.
//////////////////////////////////////////////////////////////////////////////

static void FinalizeTTYStreams()
{
	delete [] g_pTTYStreams;
	g_pTTYStreams = NULL;

	g_nNumTTYStreams = 0;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    InitializeTTYStreams
///  @brief     Initializes the global TTYSTREAM data.
//////////////////////////////////////////////////////////////////////////////

static bool InitializeTTYStreams()
{
	SNRESULT snr = SN_S_OK;

	if (g_pTTYStreams != NULL)
		return true;

	// Get number of available TTY channels.
	if (SN_FAILED( snr = SNPS3ListTTYStreams(g_TargetOpt.hTargetId, &g_nNumTTYStreams, NULL) ))
	{
		PrintError(snr, L"Failed to get count of TTY streams");
		return false;
	}

	g_pTTYStreams = new TTYSTREAM[g_nNumTTYStreams];

	// Get list of available TTY channels.
	if (SN_FAILED( snr = SNPS3ListTTYStreams(g_TargetOpt.hTargetId, &g_nNumTTYStreams, g_pTTYStreams) ))
	{
		PrintError(snr, L"Failed to list TTY streams");
		
		FinalizeTTYStreams();

		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Read_int
///  @brief     Reads a signed integer value from the supplied command line.
//////////////////////////////////////////////////////////////////////////////

template <typename T> int Read_int(WCHAR*& arg_ptr, T* value)
{
	bool    negative = false;
	T  result = 0;

	if (*arg_ptr == '-')
	{
		negative = true;
		arg_ptr++;
	}

	if (*arg_ptr == '0' && (arg_ptr[1] == 'x' || arg_ptr[1] == 'X'))
	{
		arg_ptr += 2;

		for (;;)
		{
			if (*arg_ptr >= '0' && *arg_ptr <= '9')
				result = (result << 4) | (*arg_ptr++ - '0');
			else if (*arg_ptr >= 'A' && *arg_ptr <= 'F')
				result = (result << 4) | (*arg_ptr++ - 'A' + 10);
			else if (*arg_ptr >= 'a' && *arg_ptr <= 'f')
				result = (result << 4) | (*arg_ptr++ - 'a' + 10);
			else
				break;
		}
	}
	else
	{
		if (*arg_ptr >= '0' && *arg_ptr <= '9')
		{
			while (*arg_ptr >= '0' && *arg_ptr <= '9')
				result = result * 10 + (*arg_ptr++ - '0');
		}
		else
			return 0;
	}

	if (negative)
		*value = 0 - result;
	else
		*value = result;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Strip quotes
///  @brief     Strips quotes from the given string.
//////////////////////////////////////////////////////////////////////////////

static void StripQuotes(WCHAR*& arg_ptr)
{
	if (*arg_ptr == '"')
	{
		++arg_ptr;
		arg_ptr[wcslen(arg_ptr)] = '\0';
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ParseAbortFlag
///  @brief     Parse the abort-seeking flag
//////////////////////////////////////////////////////////////////////////////

static int ParseAbortFlag(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_CHECK_FOR_ABORT;

	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Abort text not specified\n");
			return -1;
		}
	}

	StripQuotes(arg_ptr);
	g_TargetOpt.pszAbortString = arg_ptr;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_a
///  @brief     Parse the abort-seeking flag
//////////////////////////////////////////////////////////////////////////////

static int Parse_a(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nAbortExitCode = PS3RUN_EXIT_ERROR;
	return ParseAbortFlag(arg_num, arg_ptr, argc, argv);
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_e
///  @brief     Parse the abort-seeking flag
//////////////////////////////////////////////////////////////////////////////

static int Parse_e(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nAbortExitCode = PS3RUN_EXIT_OK;
	return ParseAbortFlag(arg_num, arg_ptr, argc, argv);
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_t
///  @brief     Parse the target selection command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_t(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Target name not specified\n");
			return -1;
		}
	}

	StripQuotes(arg_ptr);
	g_TargetOpt.wcsTgtName = arg_ptr;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_d
///  @brief     Parse the disconnect command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_d(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nOptFlags |= PS3RUN_OPT_DISCONNECT;

	if (*arg_ptr == 'a')
	{
		arg_ptr++;	//skip 'a'
		g_TargetOpt.nOptFlags |= PS3RUN_OPT_ALWAYS_DISCONNECT;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_r
///  @brief     Parse the reset command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_r(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_RESET;

	Read_int(arg_ptr, &g_TargetOpt.nResetParam);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_s
///  @brief     Parse the reset command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_s(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_POWER_CYCLE;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_u
///  @brief     Undo any reset parameter changes.
//////////////////////////////////////////////////////////////////////////////

static int Parse_u(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_RESTORE_BOOT_PARAMS;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_b
///  @brief     Parse the boot flag.
//////////////////////////////////////////////////////////////////////////////

static int Parse_b(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Boot param not specified\n");
			return -1;
		}
	}

	Read_int(arg_ptr, &g_TargetOpt.nBootParam);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_m
///  @brief     Parse the boot flag.
//////////////////////////////////////////////////////////////////////////////

static int Parse_m(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Boot mask not specified\n");
			return -1;
		}
	}

	Read_int(arg_ptr, &g_TargetOpt.nBootMask);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_w
///  @brief     Parse the wait command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_w(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_WAIT_ELF_EXIT;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_y
///  @brief     Parse the absolute time-out command flag
//////////////////////////////////////////////////////////////////////////////

static int Parse_y(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_CHECK_ABS_TIMEOUT;

	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Timeout not specified\n");
			return -1;
		}
	}

	// Read the timeout value
	Read_int(arg_ptr, &g_TargetOpt.uAbsTimeoutDuration);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_z
///  @brief     Parse the time-out command flag
//////////////////////////////////////////////////////////////////////////////

static int Parse_z(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_CHECK_REL_TIMEOUT;

	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Timeout not specified\n");
			return -1;
		}
	}

	// Read the timeout value
	Read_int(arg_ptr, &g_TargetOpt.uTimeoutDuration);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_n
///  @brief     Parse the negative command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_n(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	switch (*arg_ptr++)
	{
	case 'd':
		g_TargetOpt.nOptFlags &= ~PS3RUN_OPT_DISCONNECT;
		break;

	case 'r':
		g_TargetOpt.nCmdFlags &= ~PS3RUN_CMD_RESET;
		break;

	default:
		printf("Error - Unknown option %s specified\n", argv[arg_num]);
		return -1;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_q
///  @brief     Parse the quiet operation command flag.
//////////////////////////////////////////////////////////////////////////////

static int Parse_q(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nOptFlags |= PS3RUN_OPT_QUIET;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_c
///  @brief     Parse the TTY channel redirect command.
//////////////////////////////////////////////////////////////////////////////

static int Parse_c(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_SEND_TTY;

	StripQuotes(arg_ptr);

	if (*arg_ptr == 0)
	{
		printf("Error - TTY channel not specified\n");
		return -1;
	}

	g_TargetOpt.pszTTYOutChannel = arg_ptr;

	return 1;
}

int Parse_k(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_FORCE_DISCONNECT;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_i
///  @brief     Parse the TTY import command.
//////////////////////////////////////////////////////////////////////////////

static int Parse_i(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - TTY import file not specified\n");
			return -1;
		}
	}

	StripQuotes(arg_ptr);

	if (*arg_ptr == 0)
	{
		printf("Error - TTY import file not specified\n");
		return -1;
	}

	//
	// Check the file is valid.
	//
	if (0 == wcscmp(arg_ptr, L"-"))
	{
		g_TargetOpt.pTTYImportSource = stdin;
	}
	else
	{
		WCHAR *pchFilePart = NULL;
		WCHAR szFile[MAX_PATH] = {'\0'};

		DWORD dwRes = GetFullPathNameW(
			arg_ptr, 
			_countof(szFile), 
			szFile, 
			&pchFilePart);

		if (dwRes == 0 || dwRes > _countof(szFile))
		{
			printf("Error - Illegal file \"%s\" specified\n", arg_ptr);
			return -1;
		}

		g_TargetOpt.pTTYImportSource = _wfopen(szFile, L"r");

		if (g_TargetOpt.pTTYImportSource == NULL)
		{
			printf("Error - Illegal file \"%s\" specified\n", szFile);
			return -1;
		}
	}

	g_TargetOpt.nOptFlags |= PS3RUN_OPT_SEND_TTY_FROM_FILE;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_l
///  @brief     Parse the LOG flag.
//////////////////////////////////////////////////////////////////////////////

static int Parse_l(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
	}

	StripQuotes(arg_ptr);

	//
	// Check the file is valid.
	//
	DWORD dwRes = 0;
	WCHAR szFile[MAX_PATH] = {'\0'};
	if (*arg_ptr != L'-')
	{
		WCHAR *pchFilePart = NULL;

		dwRes = GetFullPathNameW(arg_ptr, _countof(szFile), szFile, &pchFilePart);
		if (dwRes > _countof(szFile))
		{
			printf("Error - Illegal file \"%s\" specified\n", arg_ptr);
			return -1;
		}
	}

	// No file specified? use an hardcoded one...
	if (dwRes == 0)
	{
		g_TargetOpt.strLogFileName = L"C:\\temp\\ps3run.txt";
	}
	else
	{
		g_TargetOpt.strLogFileName = szFile;
	}

	g_TargetOpt.nOptFlags |= PS3RUN_OPT_LOG_TO_FILE;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_p
///  @brief     Parse the receive TTY command flag.
//////////////////////////////////////////////////////////////////////////////

static int Parse_p(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_GET_TTY;

	StripQuotes(arg_ptr);

	g_TargetOpt.arrTTYChannelNames.push_back(arg_ptr);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_f
///  @brief     Parse the file serving command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_f(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - File serving directory not specified\n");
			return -1;
		}
	}

	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_SET_FS_DIR;
	g_TargetOpt.pszFSDir = arg_ptr;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_h
///  @brief     Parse the home directory command flags.
//////////////////////////////////////////////////////////////////////////////

static int Parse_h(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	// Remove any white space.
	while (*arg_ptr == ' ')
		arg_ptr++;

	// Check if we reached the end of this argument.
	if (*arg_ptr == 0)
	{
		if (++arg_num < argc)
		{
			arg_ptr = argv[arg_num];
		}
		else
		{
			printf("Error - Home directory not specified\n");
			return -1;
		}
	}

	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_SET_HOME_DIR;
	g_TargetOpt.pszHomeDir = arg_ptr;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_v
///  @brief     Parse the SDK check command option.
//////////////////////////////////////////////////////////////////////////////

static int Parse_v(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_CHECK_SDK_BF_RUN;

	return 1;
}

static int Parse_add(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	char * pszName = NULL;
	char * pszType = NULL;

	std::string strType = WCharToUTF8(arg_ptr);
	size_t n = strType.find(',');
	if (n != std::string::npos)
	{
		g_TargetOpt.strTargetAdd = strType.substr(n+1, strType.size());
		strType.erase(n, strType.size());
	}

	g_TargetOpt.strTargetType = strType;

	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_ADD_TARGET;

	return 1;
}

static int Parse_del(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	StripQuotes(arg_ptr);
	g_TargetOpt.strTargetDel = WCharToUTF8(arg_ptr);
	g_TargetOpt.nCmdFlags |= PS3RUN_CMD_DEL_TARGET;
	return 1;
}

static int Parse_pri(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	Read_int(arg_ptr, &g_TargetOpt.nModulePriority);

	return 1;
}

static int Parse_dbg(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nDebugFlags |= SNPS3_LOAD_FLAG_ENABLE_DEBUGGING;
	return 1;
}

static int Parse_con(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.strConnectionParameters = WCharToUTF8(arg_ptr);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    Parse_sfo
///  @brief     Parse param.sfo directory.
//////////////////////////////////////////////////////////////////////////////

static int Parse_sfo(int& arg_num, WCHAR*& arg_ptr, int argc, WCHAR** argv)
{
	g_TargetOpt.nDebugFlags |= SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USECUSTOMDIR;
	g_TargetOpt.nDebugFlags &= ~SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USEELFDIR;
	g_TargetOpt.paramSfoDir = WCharToUTF8(arg_ptr);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ParseCmdLine
///  @brief     Parses the program arguments for command options.
//////////////////////////////////////////////////////////////////////////////

static int ParseCmdLine(int argc, WCHAR** argv)
{
	int    rval = 0;
	int    arg_num = 1;
	WCHAR*  arg_ptr = argv[arg_num];
	bool bDoubleDash = false;

	while (arg_num < argc && (argv[arg_num][0] == L'-' || argv[arg_num][0] == L'/'))
	{
		arg_ptr = argv[arg_num] + 1;

		if (*arg_ptr == L'-')
		{
			arg_ptr++;
			if (!bDoubleDash)
				bDoubleDash = true;
			else
			{
				Usage();
				return PS3RUN_EXIT_ERROR;
			}
		}

		LPFNPARSEPROC proc = NULL;
		int i = 0;
		while (gCmdLineParam[i].proc)
		{
			if (bDoubleDash && gCmdLineParam[i].bDoubleDash)
			{
				if (gCmdLineParam[i].word == arg_ptr)
				{
					arg_num++;
					arg_ptr = argv[arg_num];
					proc = gCmdLineParam[i].proc;
					break;
				}
			}

			if (!bDoubleDash && !gCmdLineParam[i].bDoubleDash)
			{
				if (gCmdLineParam[i].word[0] == *arg_ptr)
				{
					arg_ptr++;
					proc = gCmdLineParam[i].proc;
					break;
				}
			}
			i++;
		}
		bDoubleDash = false;
		rval = -1;
		if (proc)
		{
			rval = proc(arg_num, arg_ptr, argc, argv);
		}
		else
		{
			Usage();
		}

		if (rval < 0)
			return PS3RUN_EXIT_ERROR;

		arg_num++;
	}

	if (arg_num < argc)
	{
		WCHAR *pchFilePart = NULL;

		DWORD dwRes = 1;

		// Get the full path for the ELF.
		if (0 == _wcsnicmp(argv[arg_num], L"../dev_bdvd/", wcslen(L"../dev_bdvd/")))
		{
			wcsncpy(g_TargetOpt.szElfFileName, argv[arg_num], _countof(g_TargetOpt.szElfFileName));
		}
		else if (0 == _wcsnicmp(argv[arg_num], L"../dev_hdd0/", wcslen(L"../dev_hdd0/")))
		{
			wcsncpy(g_TargetOpt.szElfFileName, argv[arg_num], _countof(g_TargetOpt.szElfFileName));
		}
		else if (0 == _wcsnicmp(argv[arg_num], L"../app_home/", wcslen(L"../app_home/")))
		{
			wcsncpy(g_TargetOpt.szElfFileName, argv[arg_num], _countof(g_TargetOpt.szElfFileName));
		}
		else
		{
			dwRes = GetFullPathName(argv[arg_num], _countof(g_TargetOpt.szElfFileName), g_TargetOpt.szElfFileName, &pchFilePart);
		}

		if (dwRes == 0 || dwRes > _countof(g_TargetOpt.szElfFileName))
		{
			wprintf(L"Error - Illegal file \"%s\" specified\n", argv[arg_num]);
			return PS3RUN_EXIT_ERROR;
		}

		arg_num++; //Skip ELF filename...
	}

	//argc for downloaded ELF is no. of arguments remaining...
	g_TargetOpt.argc = argc - arg_num;
	if (arg_num < argc)
		g_TargetOpt.argv = &argv[arg_num];
	else
		g_TargetOpt.argv = NULL;

	return PS3RUN_EXIT_OK;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    FindAbort
///  @brief     Process TTY text looking for abort text.
//////////////////////////////////////////////////////////////////////////////

static bool FindAbort(const WCHAR* pszText)
{
	g_TTYBuffer += pszText;

	// Search the TTY for the abort string
	if (wcsstr(g_TTYBuffer.c_str(), g_TargetOpt.pszAbortString))
	{
		g_bAbortTextFound = true;
		return true;
	}

	size_t nAbortStrLen = wcslen(g_TargetOpt.pszAbortString);

	if (g_TTYBuffer.length() > nAbortStrLen)
	{
		g_TTYBuffer.erase(0, g_TTYBuffer.length() - nAbortStrLen);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessTTYCallback
///  @brief     Processes TTY events.
//////////////////////////////////////////////////////////////////////////////

static void __stdcall ProcessTTYCallback(HTARGET /*hTarget*/, UINT uiType, UINT uiStreamId, 
						SNRESULT /*Result*/, UINT uiLength, BYTE *pData, void* /*pUser*/)
{
	if (uiType == SN_EVENT_TTY)
	{
		char* szData = (char*) _alloca(uiLength+1);
		strncpy(szData, (char*) pData, uiLength);
		szData[uiLength] = '\0';

		char* pCtrlZ = strchr(szData, 26);

		if (pCtrlZ)
			*pCtrlZ = 0;

		// Output the received TTY text.
		printf("%s", szData);

		// Test for abort string.
		if ((g_TargetOpt.nCmdFlags & PS3RUN_CMD_CHECK_FOR_ABORT) && !g_bAbortTextFound && FindAbort(CUTF8ToWChar(szData)))
		{
			g_bQuit = true;
			g_nExitCode = g_TargetOpt.nAbortExitCode;

			PrintMessage(ML_INFO, L"Found abort text. PS3Run will exit after the current commands are completed.\n");
		}

		// If Ctrl Z found, exit the message pump.
		if (pCtrlZ)
		{
			if (*(pCtrlZ - 1) != '\n')
				puts("");

			g_bQuit = true;
		}

		fflush(stdout);
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    DisconnectTTYEvents
///  @brief     Unregister from TTY event notifications.
//////////////////////////////////////////////////////////////////////////////

static bool DisconnectTTYEvents(void)
{
	// Cancel TTY notifications (-1 is all channels).
	for (size_t i = 0 ; i < g_TargetOpt.arrTTYChannels.size() ; ++i)
	{
		SNRESULT snr;
		if (SN_FAILED( snr = SNPS3CancelTTYEvents(g_TargetOpt.hTargetId, g_TargetOpt.arrTTYChannels[i]) ))
		{
			PrintError(snr, L"Failed to cancel TTY notifications on %s channel", 
				(g_TargetOpt.arrTTYChannels[i] == -1) ? L"every" : CUTF8ToWChar(g_pTTYStreams[g_TargetOpt.arrTTYChannels[i]].szName));
		}
	}

	// Deallocate memory for TTY channels.
	FinalizeTTYStreams();

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessTargetSpecificEvent
///  @brief     Process target specific events (see TargetEventCallback).
//////////////////////////////////////////////////////////////////////////////

static void ProcessTargetSpecificEvent(UINT uDataLen, BYTE *pData)
{
	SNPS3_DBG_EVENT_HDR *pDbgHeader = (SNPS3_DBG_EVENT_HDR *)pData;
	SNPS3_DBG_EVENT_DATA *pDbgData = (SNPS3_DBG_EVENT_DATA *)(pData + sizeof(SNPS3_DBG_EVENT_HDR));

	switch (pDbgData->uEventType)
	{
	case SNPS3_DBG_EVENT_PROCESS_EXIT:
		{
			UINT64 u64ExitCode = pDbgData->ppu_process_exit.uExitCode;
			UINT32 u32ExitCode = (UINT32)(u64ExitCode >> 32);

			g_nExitCode = ntohl(u32ExitCode);
			g_bQuit = true;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessTargetEvent
///  @brief     Prints details of received target events.
//////////////////////////////////////////////////////////////////////////////

static void ProcessTargetEvent(HTARGET hTarget, UINT uDataLen, BYTE *pData)
{
	UINT uDataRemaining = uDataLen;

	while (uDataRemaining)
	{
		SN_EVENT_TARGET_HDR *pHeader = (SN_EVENT_TARGET_HDR *)pData;

		switch (pHeader->uEvent)
		{
		case SN_TGT_EVENT_TARGET_SPECIFIC:
			ProcessTargetSpecificEvent(pHeader->uSize, pData + sizeof(SN_EVENT_TARGET_HDR));
			break;
		}

		uDataRemaining -= pHeader->uSize;
		pData += pHeader->uSize;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    TargetEventCallback
///  @brief     Process target event notifications.
//////////////////////////////////////////////////////////////////////////////

static void __stdcall TargetEventCallback(HTARGET hTarget, UINT uEventType, UINT /*uEvent*/, 
						 SNRESULT snr, UINT uDataLen, BYTE *pData, void* /*pUser*/)
{
	if (SN_FAILED( snr ))
		return;

	switch (uEventType)
	{
	case SN_EVENT_TARGET:
		ProcessTargetEvent(hTarget, uDataLen, pData);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    EnumCallBack
///  @brief     Callback function to enumerate available targets.
//////////////////////////////////////////////////////////////////////////////

static int __stdcall EnumCallBack(HTARGET hTarget)
{
	SNPS3TargetInfo ti;
	std::auto_ptr<SNPS3TargetInfo> pti(new SNPS3TargetInfo);

	if (pti.get())
	{
		ti.hTarget = hTarget;
		ti.nFlags = SN_TI_TARGETID;

		if (SN_S_OK == SNPS3GetTargetInfo(&ti))
		{
			// Store target parameters.
			pti->hTarget = hTarget;
			pti->pszName = _strdup(ti.pszName);
			pti->pszHomeDir = _strdup(ti.pszHomeDir);
			pti->pszFSDir = _strdup(ti.pszFSDir);

			// Store this target.
			g_Targets.push_back(pti.release());
		}
		else
		{
			// Terminate enumeration.
			return 1;
		}
	}

	// Carry on with enumeration.
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    FreeTargetList
///  @brief     Loop through enumerated targets and free allocated memory.
//////////////////////////////////////////////////////////////////////////////

static void FreeTargetList()
{
	for (int i = 0; i < (int)g_Targets.size(); i++)
	{
		free((void*)g_Targets[i]->pszName);
		free((void*)g_Targets[i]->pszFSDir);
		free((void*)g_Targets[i]->pszHomeDir);

		delete g_Targets[i];
	}

	g_Targets.clear();
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessSetFSDir
///  @brief     Changes the file serving directory.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessSetFSDir(void)
{
	SNRESULT snr = SN_S_OK;

	SNPS3TargetInfo ti;

	WCHAR  szBuffer[MAX_PATH];
	WCHAR* pszFilePart = 0;

	DWORD nRes = GetFullPathName(g_TargetOpt.pszFSDir, MAX_PATH, szBuffer, &pszFilePart);

	if (nRes == 0 || nRes > MAX_PATH)
	{
		PrintMessage(ML_ERROR, L"Illegal file serving directory \"%s\" specified\n",		
			g_TargetOpt.pszFSDir);

		return false;
	}
    std::string strFSDir;
    WCharToUTF8(szBuffer, strFSDir);
	ti.hTarget  = g_TargetOpt.hTargetId;
	ti.nFlags   = SN_TI_FILESERVEDIR | SN_TI_TARGETID;
	ti.pszFSDir = strFSDir.c_str();

	if (SN_FAILED(snr = SNPS3SetTargetInfo(&ti)))
	{
		PrintError(snr, L"Failed to set file serving directory");
		return false;
	}

	PrintMessage(ML_INFO, L"File serving directory changed to \"%s\"\n", szBuffer);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessSDKCheck
///  @brief     Checks if SDK/ProDG and target firmware match
//////////////////////////////////////////////////////////////////////////////

static bool ProcessSDKCheck( void )
{
	char *pstrAPIVersion = NULL;
	UINT64 uSDKVersion = (UINT64)-1;
	SNRESULT snr = SN_S_OK;

	if (SN_FAILED(snr = SNPS3GetAPIVersion(&pstrAPIVersion) || pstrAPIVersion == NULL))
	{
		PrintError( snr, L"Unable to get API SDK Version");
		return false;		
	}

	if (SN_FAILED(snr = SNPS3GetSDKVersion(g_TargetOpt.hTargetId, &uSDKVersion)))
	{
		PrintError(snr, L"Unable to get target SDK Version");
		return false;		
	}

	//DECR-1000 units will connect in standby, but if the target has not been connected before the sdk is undefined .
	if ((uSDKVersion == (UINT64)-1))
	{
		PrintMessage(ML_INFO, L"Initial attempt to test SDK version failed, attempting to power-on target for more info.\n");

		if (SN_FAILED(snr = SNPS3PowerOn(g_TargetOpt.hTargetId)))
		{
			PrintError(snr, L"Unable to power target on.  Please power on manually and retry.\n");
			return false;
		}

		if (SN_FAILED(snr = SNPS3GetSDKVersion(g_TargetOpt.hTargetId, &uSDKVersion)))
		{
			PrintError(snr, L"Unable to get target SDK Version");
			return false;		
		}

		if ((uSDKVersion == (UINT64)-1))
		{
			PrintError(SN_E_COMMS_ERR, L"Unable to get target SDK Version");
			return false;
		}

	}

	UINT8 uSDKMajorVersion = static_cast<UINT8>(SNPS3_GET_SDK_VER_MAJOR(uSDKVersion));
	UINT8 uSDKMinorVersion = static_cast<UINT8>(SNPS3_GET_SDK_VER_MINOR(uSDKVersion));

	//minor version number is stored as an individual number, so in 2.80, the minor will be 8,instead of 80
	//need to check for this otherwise the minor value check will fail below..
	if (uSDKMinorVersion < 10)
		uSDKMinorVersion *= 10;

	//pstrAPIVersion format - major/minor are in the same section, separated from rest of the info by a comma..		
	UINT16 uAPIMajorMinorVersion = atoi(pstrAPIVersion);
	if (errno == ERANGE) //atoi overflow error
	{
		PrintError (SN_E_BAD_PARAM, L"Overflow of API SDK version: %d", uAPIMajorMinorVersion);
		return false;
	}

	UINT8 uAPIMajorVesion = uAPIMajorMinorVersion / 100;
	UINT8 uAPIMinorVesion = uAPIMajorMinorVersion - (100 * uAPIMajorVesion);

	if (!(uSDKMajorVersion == uAPIMajorVesion && uSDKMinorVersion == uAPIMinorVesion))
	{
		PrintError(SN_E_COMMS_ERR, L"SDK mismatch. Target SDK:%d.%02d, ProDG SDK:%d.%02d", 
						uSDKMajorVersion, uSDKMinorVersion, 
						uAPIMajorVesion , uAPIMinorVesion);
		return false;						
	}

	PrintMessage(ML_INFO, L"Target and API SDK's match: %d.%02d\n", uSDKMajorVersion, uSDKMinorVersion);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessSetHomeDir
///  @brief     Changes the home directory.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessSetHomeDir(void)
{
	SNRESULT snr = SN_S_OK;

	SNPS3TargetInfo ti;

	WCHAR  szBuffer[MAX_PATH];
	WCHAR* pszFilePart = 0;

	DWORD nRes = GetFullPathName(g_TargetOpt.pszHomeDir, MAX_PATH, szBuffer, &pszFilePart);

	if (nRes == 0 || nRes > MAX_PATH)
	{
		PrintMessage(ML_ERROR, L"Illegal home directory \"%s\" specified\n",
			g_TargetOpt.pszHomeDir);

		return false;
	}

	ti.hTarget    = g_TargetOpt.hTargetId;
    ti.nFlags     = SN_TI_HOMEDIR | SN_TI_TARGETID;
    std::string strHomeDir;
    WCharToUTF8(szBuffer, strHomeDir);
	ti.pszHomeDir = strHomeDir.c_str();

	if (SN_FAILED( snr = SNPS3SetTargetInfo(&ti) ))
	{
		PrintError(snr, L"Failed to set home directory");
		return false;
	}

	PrintMessage(ML_INFO, L"Home directory changed to \"%s\"\n",	szBuffer);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    GetResetParameters
///  @brief     Get Target Managers current reset/boot parameters.
//////////////////////////////////////////////////////////////////////////////

static bool GetResetParameters(RESET_PARAMETERS& rp)
{
	SNRESULT snr = SNPS3GetResetParameters(g_TargetOpt.hTargetId,
		&rp.Boot.uValue, &rp.Boot.uMask,
		&rp.Reset.uValue, &rp.Reset.uMask,
		&rp.System.uValue, &rp.System.uMask);

	if (SN_FAILED(snr))
	{
		PrintError(snr, L"Failed to get reset parameters");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    SetBootParameters
///  @brief     Set Target Managers boot parameters.
//////////////////////////////////////////////////////////////////////////////

static bool SetBootParameters(RESET_PARAMETERS const& rp)
{
	SNRESULT snr = SNPS3SetBootParameter(g_TargetOpt.hTargetId,
		rp.Boot.uValue, rp.Boot.uMask);

	if (SN_FAILED(snr))
	{
		PrintError(snr, L"Failed to set boot parameters");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessReset
///  @brief     Resets the target with the specified boot parameters.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessReset(void)
{
	SNRESULT snr = SNPS3ResetEx(g_TargetOpt.hTargetId, 
		g_TargetOpt.nBootParam, g_TargetOpt.nBootMask,
		g_TargetOpt.nResetParam, (UINT64) -1,
		0, 0);

	if (SN_FAILED( snr ))
	{
		PrintError(snr, L"Failed to reset target");

		if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_POWER_CYCLE)
		{
			PrintMessage(ML_INFO, L"Power cycling\n");

			snr = SNPS3PowerOff(g_TargetOpt.hTargetId, 1);

			if (SN_FAILED( snr ))
			{
				PrintError(snr, L"Failed to power off target");
				return false;
			}

			// Need to sleep for a bit as targets report they are off
			// before they actually are
			::Sleep(2000);

			snr = SNPS3PowerOn(g_TargetOpt.hTargetId);

			if (SN_FAILED( snr ))
			{
				PrintError(snr, L"Failed to power on target");
				return false;
			}

			return true;
		}

		return false;
	}

	PrintMessage(ML_INFO, L"Reset target with %I64d (bootparam:0x%I64x, bootmask:0x%I64x)\n", g_TargetOpt.nResetParam, g_TargetOpt.nBootParam, g_TargetOpt.nBootMask);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ResolveTTYStreamName
///  @brief     Converts a textual stream identifier into a numeric identifier.
//////////////////////////////////////////////////////////////////////////////

static UINT ResolveTTYStreamName(const WCHAR *pszStream)
{
	if (pszStream != NULL)
	{
		// See if the stream identifier matches a stream name.
		for (int i = 0 ; i < (int)g_nNumTTYStreams ; ++i)
		{
			if (_wcsicmp(CUTF8ToWChar(g_pTTYStreams[i].szName), pszStream) == 0)
				return g_pTTYStreams[i].nStreamIdx;
		}

		// Otherwise fallback to assuming it's an index.
		UINT nStream = -1;

		if (1 == swscanf(pszStream, L"%u", &nStream))
		{
			for (int i = 0 ; i < (int)g_nNumTTYStreams ; ++i)
			{
				if (g_pTTYStreams[i].nStreamIdx == nStream)
					return nStream;
			}
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessRequestTTY
///  @brief     Registers for TTY notifications from the target.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessRequestTTY(void)
{
	if (InitializeTTYStreams() == false)
		return false;

	for (size_t i = 0 ; i < g_TargetOpt.arrTTYChannelNames.size() ; ++i)
	{
		UINT uChannel = ResolveTTYStreamName(g_TargetOpt.arrTTYChannelNames[i]);
		g_TargetOpt.arrTTYChannels.push_back(uChannel);
	}

	if (std::find(g_TargetOpt.arrTTYChannels.begin(), g_TargetOpt.arrTTYChannels.end(), -1) != g_TargetOpt.arrTTYChannels.end())
	{
		g_TargetOpt.arrTTYChannels.clear();
		g_TargetOpt.arrTTYChannels.push_back(-1);
	}

	std::sort(g_TargetOpt.arrTTYChannels.begin(), g_TargetOpt.arrTTYChannels.end());
	g_TargetOpt.arrTTYChannels.erase(
		std::unique(g_TargetOpt.arrTTYChannels.begin(), g_TargetOpt.arrTTYChannels.end()),
		g_TargetOpt.arrTTYChannels.end());

	for (size_t i = 0 ; i < g_TargetOpt.arrTTYChannels.size() ; ++i)
	{
		UINT uChannel = g_TargetOpt.arrTTYChannels[i];

		// Request TTY notification (-1 is all channels).
		SNRESULT snr;
		if (SN_FAILED( snr = SNPS3RegisterTTYEventHandler(g_TargetOpt.hTargetId, uChannel, ProcessTTYCallback, NULL) ))
		{
			PrintError(snr, L"Failed to register for TTY notifications on channel %d", uChannel);
		}

		PrintMessage(ML_INFO, L"Registered for TTY output on %s channel\n", (uChannel == -1) ? L"every" : CUTF8ToWChar(g_pTTYStreams[uChannel].szName));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessSendTTY
///  @brief     Set-up for sending TTY text.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessSendTTY(void)
{
	if (InitializeTTYStreams() == false)
		return false;

	g_TargetOpt.nTTYOutChannel = ResolveTTYStreamName(g_TargetOpt.pszTTYOutChannel);

	if (g_TargetOpt.nTTYOutChannel == -1)
	{
		PrintMessage(ML_ERROR, L"Failed to resolve TTY stream '%s'\n", g_TargetOpt.pszTTYOutChannel);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessAddTarget
///  @brief     Add target from command line.
//////////////////////////////////////////////////////////////////////////////
static bool ProcessAddTarget(void)
{
	std::string targetName;
	if (g_TargetOpt.strTargetAdd.empty())
		targetName = WCharToUTF8(g_TargetOpt.wcsTgtName);
	else
		targetName = g_TargetOpt.strTargetAdd;

	TMAPI_TCPIP_CONNECT_PROP connProperties;
	memset(&connProperties, 0x00, sizeof(connProperties));
	UINT32 connSize = 0;

	if (!g_TargetOpt.strConnectionParameters.empty())
	{
		connSize = sizeof(connProperties);

		std::string strConnection = g_TargetOpt.strConnectionParameters;

		size_t n = strConnection.find(':');
		if (n != std::string::npos)
		{
			connProperties.uPort = atoi(strConnection.substr(n+1, strConnection.size()).c_str());
			strConnection.erase(n, strConnection.size());
		}
		strcpy_s(connProperties.szIPAddress, _countof(connProperties.szIPAddress)-1, strConnection.c_str());
	}

	SNRESULT res = SNPS3AddTarget(targetName.c_str(), g_TargetOpt.strTargetType.c_str(), connSize, (BYTE*)&connProperties, &g_TargetOpt.hTargetId);
	if (SN_FAILED(res))
	{
		PrintError(res, L"Failed to add target");
		return false;
	}
	PrintMessage(ML_INFO, L"Target <%s> successfully created: id=%d\n", CUTF8ToWChar(targetName).c_str(), g_TargetOpt.hTargetId);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessDeleteTarget
///  @brief     Delete target from command line.
//////////////////////////////////////////////////////////////////////////////
static bool ProcessDeleteTarget(void)
{
	std::string targetName;
	if (g_TargetOpt.strTargetDel.empty())
		targetName = WCharToUTF8(g_TargetOpt.wcsTgtName);
	else
		targetName = g_TargetOpt.strTargetDel;

	HTARGET hTarget = -1;
	SNRESULT err = SNPS3GetTargetFromName(targetName.c_str(), &hTarget);
	if (SN_FAILED(err))
	{
		PrintError(err, L"Failed to delete target <%s>", CUTF8ToWChar(targetName).c_str());
		return false;
	}

	err = SNPS3DeleteTarget(hTarget);
	if (SN_FAILED(err))
	{
		PrintError(err, L"Failed to delete target <%s>", CUTF8ToWChar(targetName).c_str());
		return false;
	}
	PrintMessage(ML_INFO, L"Target <%s> successfully deleted", CUTF8ToWChar(targetName).c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    GetChar
///  @brief     Get a character from the keyboard
//////////////////////////////////////////////////////////////////////////////

static char GetChar()
{
	if (_kbhit())
		return _getch();

	return '\0';
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    GetLine
///  @brief     Get a line from the file
///  @bEof      Reference value to indicate if the end of the file has been read
//////////////////////////////////////////////////////////////////////////////

static std::string GetLine(bool& bEof)
{
	std::string strLine;

	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_SEND_TTY)
	{
		if (g_TargetOpt.nOptFlags & PS3RUN_OPT_SEND_TTY_FROM_FILE && g_TargetOpt.pTTYImportSource != NULL)
		{
			char line[MAX_LINE_SIZE];
			if (fgets (line , MAX_LINE_SIZE , g_TargetOpt.pTTYImportSource) != NULL)
			{
				strLine += line;
			} else if (feof(g_TargetOpt.pTTYImportSource))
			{
				strLine += EOF;
				fclose(g_TargetOpt.pTTYImportSource);
				g_TargetOpt.pTTYImportSource = NULL;
				bEof = true;
			}
		}
	}

	return strLine;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ClearPendingMessages
///  @brief     Clears all the messages from the queue.
//////////////////////////////////////////////////////////////////////////////

static SNRESULT ClearPendingMessages(void)
{
	SNRESULT snr = SN_S_OK;
	int nKicks = 0;

	do
	{
		snr = SNPS3Kick();
		++nKicks;

	} while (snr == SN_S_OK && !g_bQuit);

	if (nKicks > 1)
		UpdateTimeout();

	return snr;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ClearMessagesBeforeExit
///  @brief     Clears all the messages from the queue after a short delay.
//////////////////////////////////////////////////////////////////////////////

static void ClearMessagesBeforeExit(void)
{
	Sleep(500);

	while (SNPS3Kick() == SN_S_OK)
		/* Do nothing */;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    SendTTY
///  @brief     Wrapper around SNPS3SendTTY.
//////////////////////////////////////////////////////////////////////////////

static bool SendTTY(const char* tty)
{
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_SEND_TTY)
	{
		if (SN_FAILED( SNPS3SendTTY(g_TargetOpt.hTargetId, g_TargetOpt.nTTYOutChannel, tty) ))
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessEvents
///  @brief     Enters a message pump to receive event notifications.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessEvents(void)
{
	// Initialize the the timeout time
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_CHECK_REL_TIMEOUT) 
	{
		 _time64(&g_TimeoutTime);
		 g_TimeoutTime += g_TargetOpt.uTimeoutDuration;
	}

	if (IS_VERBOSE)
	{
		if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_SEND_TTY)
		{
			if (g_TargetOpt.nOptFlags & PS3RUN_OPT_SEND_TTY_FROM_FILE)
			{
				PrintMessage(ML_INFO, L"Sending input source to channel '%s'\n", g_TargetOpt.pszTTYOutChannel);
			}
			else
			{
				PrintMessage(ML_INFO, L"Sending keystrokes to channel '%s'\n", g_TargetOpt.pszTTYOutChannel);
			}
		}

		PrintMessage(ML_INFO, L"Listening for events, press ESC to quit...\n");
	}

	SNRESULT snr = ClearPendingMessages();

	while (SN_SUCCEEDED(snr) && g_bQuit == false)
	{
		if (g_TargetOpt.nOptFlags & PS3RUN_OPT_SEND_TTY_FROM_FILE)
		{
			if (GetChar() == ESCAPE_KEY)
				return true;

			bool bEndOfFile = false;
			std::string strLine = GetLine(bEndOfFile);
			const char *tty = strLine.c_str();

			if (bEndOfFile)
			{
				if (!SendTTY(tty))
				{
					PrintMessage(ML_ERROR, L"Failed to send TTY '%s'\n", CUTF8ToWChar(tty).c_str());
					return false;
				}

				// If we have read the file wait on events
				if (!(g_TargetOpt.nCmdFlags & (PS3RUN_CMD_GET_TTY|PS3RUN_CMD_WAIT_ELF_EXIT|PS3RUN_CMD_CHECK_REL_TIMEOUT|PS3RUN_CMD_CHECK_ABS_TIMEOUT)))
					return true;
			} 
			else
			{
				// Send the current line read from the file
				if (!SendTTY(tty))
				{
					PrintMessage(ML_ERROR, L"Failed to send TTY '%s'\n", CUTF8ToWChar(tty).c_str());
					return false;
				}
			}

		} 
		else
		{
			char tty[] = { GetChar(), '\0'};

			switch (tty[0])
			{
			case '\0':
				// Nothing to send and we've cleared our messages 
				// so take a nap (prevents CPU hogging).
				CheckTimeout();
				::Sleep(40);
				break;

			case ESCAPE_KEY:
				return true;

			default:
				if (!SendTTY(tty))
				{
					PrintMessage(ML_ERROR, L"Failed to send TTY '%s'\n", CUTF8ToWChar(tty).c_str());
					return false;
				}
				// TODO: Do we need to sleep here to help prevent mangled output?
				break;
			}
		}

		snr = ClearPendingMessages();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessLoadELF
///  @brief     Downloads an ELF into memory and (optionally) begins running.
//////////////////////////////////////////////////////////////////////////////

static bool ProcessLoadELF(void)
{
    char   **myArgV = (char**)_alloca((g_TargetOpt.argc+1)*sizeof(char*));
    char	*pArg = NULL;
    myArgV[g_TargetOpt.argc] = NULL;

   
    std::string strAux;
    for(int i=0; i<g_TargetOpt.argc; i++)
    {
        WCharToUTF8(g_TargetOpt.argv[i], strAux);
        size_t argLength = strAux.length() +1;
        myArgV[i] = (char*)_alloca(argLength);
        strncpy_s(myArgV[i], argLength, strAux.c_str(), _TRUNCATE);
    }

	if ( (g_TargetOpt.nDebugFlags & SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USECUSTOMDIR)
		&& !(g_TargetOpt.nDebugFlags & SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USEELFDIR) )
	{
		SNRESULT result = SNPS3SetCustomParamSFOMappingDirectory(g_TargetOpt.hTargetId, g_TargetOpt.paramSfoDir.c_str());
		if (SN_FAILED(result))
			return false;
	}

	// Download the ELF into target memory.
	SNRESULT snr = SNPS3ProcessLoad(g_TargetOpt.hTargetId, g_TargetOpt.nModulePriority, CWCharToUTF8(g_TargetOpt.szElfFileName), 
									 g_TargetOpt.argc, (const char**)myArgV,
									 0, NULL, &g_TargetOpt.uProcessId, &g_TargetOpt.uThreadId, g_TargetOpt.nDebugFlags);
	if (SN_FAILED( snr ))
	{
		PrintError(snr, L"Failed to load \"%s\"", g_TargetOpt.szElfFileName);
		return false;
	}

	PrintMessage(ML_INFO, L"Loaded %s into memory, ID=0x%X\n", g_TargetOpt.szElfFileName, g_TargetOpt.uProcessId);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    FindFirstConnectedTarget
///  @brief     Finds the first connected target in the list.
//////////////////////////////////////////////////////////////////////////////

static bool FindFirstConnectedTarget(void)
{
	ECONNECTSTATUS nStatus = (ECONNECTSTATUS) -1;
	char*  pszUsage = 0;

	std::vector<SNPS3TargetInfo*>::iterator iter = g_Targets.begin();

	while (iter != g_Targets.end())
	{
		SNRESULT snr = SNPS3GetConnectStatus((*iter)->hTarget,	&nStatus, &pszUsage);

		if (SN_SUCCEEDED( snr ))
		{
			if (nStatus == CS_CONNECTED)
			{
				SNPS3TargetInfo ti;

				ti.hTarget = (*iter)->hTarget;
				ti.nFlags = SN_TI_TARGETID;

				if (SN_S_OK == SNPS3GetTargetInfo(&ti))
				{
					// Store target parameters.
					g_TargetOpt.hTargetId = ti.hTarget;
					UTF8ToWChar(ti.pszName, g_TargetOpt.wcsTgtName);

					return true;
				}
			}
		}

		iter++;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    FindFirstAvailableTarget
///  @brief     Finds the first available target in the list.
//////////////////////////////////////////////////////////////////////////////

static bool FindFirstAvailableTarget(void)
{
	UINT   nStatus = -1;
	char*  pszUsage = 0;

	std::vector<SNPS3TargetInfo*>::iterator iter = g_Targets.begin();

	while (iter != g_Targets.end())
	{
		SNRESULT snr = SNPS3Connect((*iter)->hTarget, NULL);

		if (SN_SUCCEEDED( snr ))
		{
			SNPS3TargetInfo ti;

			ti.hTarget = (*iter)->hTarget;
			ti.nFlags = SN_TI_TARGETID;

			if (SN_S_OK == SNPS3GetTargetInfo(&ti))
			{
				// Store target parameters.
				g_TargetOpt.hTargetId = ti.hTarget;
                UTF8ToWChar(ti.pszName, g_TargetOpt.wcsTgtName);
				return true;
			}
		}

		iter++;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    GetTargetFromIP
///  @brief     Gets the target handle from and IP address.
//////////////////////////////////////////////////////////////////////////////

static bool GetTargetFromIP(const WCHAR* pszIPAddr, HTARGET &hTarget)
{
	TMAPI_TCPIP_CONNECT_PROP oConnection;
	std::vector<SNPS3TargetInfo*>::iterator iter = g_Targets.begin();

	while (iter != g_Targets.end())
	{
		if (SN_SUCCEEDED( SNPS3GetConnectionInfo((*iter)->hTarget, &oConnection) ))
		{
			if (wcscmp(pszIPAddr, CUTF8ToWChar(oConnection.szIPAddress)) == 0)
			{
				hTarget = (*iter)->hTarget;
				return true;
			}
		}

		++iter;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ValidateCmdLine
///  @brief     Validate the command options from the command line.
//////////////////////////////////////////////////////////////////////////////

static int ValidateCmdLine(void)
{
	if ((g_TargetOpt.nOptFlags & PS3RUN_OPT_SEND_TTY_FROM_FILE) && !(g_TargetOpt.nCmdFlags & PS3RUN_CMD_SEND_TTY))
	{
		PrintMessage(ML_ERROR, L"No TTY channel specified.\n");
		return PS3RUN_EXIT_ERROR;
	}

	return PS3RUN_EXIT_OK;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    ProcessCmdLine
///  @brief     Processes the command options from the command line.
//////////////////////////////////////////////////////////////////////////////

static int ProcessCmdLine(void)
{
	int nReturn = PS3RUN_EXIT_OK;
	bool bExistingConnection = false;
	char* pszUsage = NULL;
	SNRESULT snr = SN_S_OK;
	RESET_PARAMETERS ResetParams;

	// Initialise PS3tmapi.
	if (SN_FAILED( snr = SNPS3InitTargetComms() ))
	{
		PrintError(snr, L"Failed to initialize PS3TM SDK");
		return PS3RUN_EXIT_ERROR;
	}

	if (SN_FAILED(snr = SNPS3EnableInternalKick(TRUE)))
	{
		PrintMessage(ML_WARN, L"Failed to enable internal kick");
	}

	// Delete target?
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_DEL_TARGET)
	{
		if (!ProcessDeleteTarget())
		{
			nReturn = PS3RUN_EXIT_ERROR;
		}

		// Command processed, clear it
		g_TargetOpt.nCmdFlags &= ~PS3RUN_CMD_DEL_TARGET;
	}

	// Add target?
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_ADD_TARGET)
	{
		if (!ProcessAddTarget())
		{
			nReturn = PS3RUN_EXIT_ERROR;
		}

		// Command processed, clear it
		g_TargetOpt.nCmdFlags &= ~PS3RUN_CMD_ADD_TARGET;
	}

	// Enumerate available targets.
	if (SN_FAILED( snr = SNPS3EnumerateTargets(EnumCallBack) ))
	{
		PrintError(snr, L"Failed to enumerate targets");
		nReturn = PS3RUN_EXIT_ERROR;
		goto l_Done;
	}

	// Attempt to get the target name from an environment variable...
	if (g_TargetOpt.wcsTgtName.empty())
	{
		wchar_t* pEnv = _wgetenv(L"PS3TARGET");
		if (pEnv)
			g_TargetOpt.wcsTgtName = pEnv;
	}

	// If there is only one target available, always use it.
	if (g_TargetOpt.wcsTgtName.empty() && g_Targets.size() == 1)
	{
		UTF8ToWChar(g_Targets[0]->pszName, g_TargetOpt.wcsTgtName);
	}
	
	//Use the default target, if no target specified
	if (g_TargetOpt.wcsTgtName.empty())
	{
		SNPS3TargetInfo targetInfo = {};
		if (SN_S_OK == SNPS3GetDefaultTarget(&targetInfo.hTarget))
		{
			targetInfo.nFlags = SN_TI_TARGETID;
			if (SN_FAILED( snr = SNPS3GetTargetInfo(&targetInfo) ))
			{
				PrintError(snr, L"Failed to get target info");
				return PS3RUN_EXIT_ERROR;
			}
			UTF8ToWChar(targetInfo.pszName,g_TargetOpt.wcsTgtName);
		}
	}

	// In the impossible case if no target has been selected, and there is no default target (there is always one...) then use the first one connected or the first one available.
	if (g_TargetOpt.wcsTgtName.empty())
	{
		if (!FindFirstConnectedTarget())
		{
			FindFirstAvailableTarget();
		}
	}

	// Retrieve the target ID from the name or failing that IP.
	if (SN_FAILED(snr = SNPS3GetTargetFromName(CWCharToUTF8(g_TargetOpt.wcsTgtName), &g_TargetOpt.hTargetId)))
	{
		if (!GetTargetFromIP(g_TargetOpt.wcsTgtName.c_str(), g_TargetOpt.hTargetId))
		{
			PrintError(snr, L"Invalidate Target %s", g_TargetOpt.wcsTgtName.c_str());
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Connect to the target.
	if (SN_FAILED(snr = SNPS3Connect(g_TargetOpt.hTargetId, NULL)))
	{
		if (snr == SN_E_TARGET_IN_USE && g_TargetOpt.nCmdFlags & PS3RUN_CMD_FORCE_DISCONNECT)
		{
			if (SN_FAILED( snr = SNPS3ForceDisconnect(g_TargetOpt.hTargetId) ))
			{
				PrintError(snr, L"Unable to force disconnect %s", CUTF8ToWChar(pszUsage).c_str());
				nReturn = PS3RUN_EXIT_ERROR;
				goto l_Done;
			}
			else
			{
				snr = SNPS3Connect(g_TargetOpt.hTargetId, NULL);
			}
		}

		if (SN_FAILED(snr))
		{
			PrintError(snr, L"Failed to connect to target");
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}
	else
	{
		bExistingConnection = (snr == SN_S_NO_ACTION);
	}

	PrintMessage(ML_INFO, L"Connected to target\n");

	// Check if set file serving directory flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_SET_FS_DIR)
	{
		if (!ProcessSetFSDir())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Check if set home directory flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_SET_HOME_DIR)
	{
		if (!ProcessSetHomeDir())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Check if capture TTY flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_GET_TTY)
	{
		// Register for TTY notifications before downloading an elf.
		// We don't want to miss any output.

		if (!ProcessRequestTTY())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_SEND_TTY)
	{
		// Initialize TTY send parameters.
		if (!ProcessSendTTY())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	if ((g_TargetOpt.nCmdFlags & PS3RUN_CMD_RESET) && (g_TargetOpt.nCmdFlags & PS3RUN_CMD_RESTORE_BOOT_PARAMS))
	{
		if (!GetResetParameters(ResetParams))
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Check if reset flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_RESET)
	{
		SNPS3ClearTTYCache(g_TargetOpt.hTargetId);

		if (!ProcessReset())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	//check if we need to compare sdk mismatch...
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_CHECK_SDK_BF_RUN)
	{
		if (ProcessSDKCheck())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Check if wait for elf flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_WAIT_ELF_EXIT)
	{
		if (SN_FAILED( snr = SNPS3RegisterTargetEventHandler(g_TargetOpt.hTargetId, TargetEventCallback, NULL) ))
		{
			PrintError(snr, L"Failed to register for target events");
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Check if a file name was supplied.
	if (g_TargetOpt.szElfFileName[0])
	{
		if (!ProcessLoadELF())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

	// Initialize the (absolute) timeout
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_CHECK_ABS_TIMEOUT)
	{
		_time64(&g_AbsoluteTimeoutTime);
		g_AbsoluteTimeoutTime += g_TargetOpt.uAbsTimeoutDuration;
	}

	// Check if we've registered for any events.
	if (g_TargetOpt.nCmdFlags & (PS3RUN_CMD_GET_TTY|PS3RUN_CMD_WAIT_ELF_EXIT|PS3RUN_CMD_SEND_TTY|PS3RUN_CMD_CHECK_REL_TIMEOUT|PS3RUN_CMD_CHECK_ABS_TIMEOUT))
	{
		// Go into event processing loop.
		if (!ProcessEvents())
		{
			nReturn = PS3RUN_EXIT_ERROR;
			goto l_Done;
		}
	}

l_Done:

	if ((g_TargetOpt.nCmdFlags & PS3RUN_CMD_RESET) && (g_TargetOpt.nCmdFlags & PS3RUN_CMD_RESTORE_BOOT_PARAMS))
	{
		// Force the setting of the original boot mode
		ResetParams.Boot.uMask = (UINT64) -1;

		if (!SetBootParameters(ResetParams))
		{
			PrintMessage(ML_WARN, L"Failed to restore reset parameters\n");
		}
	}

	// Check if wait for elf flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_WAIT_ELF_EXIT)
	{
		// Unregister target notifications before exiting.
		SNPS3CancelTargetEvents(g_TargetOpt.hTargetId);
	}

	// Check if capture TTY flag is set.
	if (g_TargetOpt.nCmdFlags & PS3RUN_CMD_GET_TTY)
	{
		// Pump pending TTY before exiting
		ClearMessagesBeforeExit();

		// Unregister TTY notifications before exiting.
		(void) DisconnectTTYEvents();
	}

	// Check if we should disconnect from the target.
	if ((g_TargetOpt.nOptFlags & PS3RUN_OPT_ALWAYS_DISCONNECT) ||
		(g_TargetOpt.nOptFlags & PS3RUN_OPT_DISCONNECT && !bExistingConnection))
	{
		SNPS3Disconnect(g_TargetOpt.hTargetId);

		if (nReturn == PS3RUN_EXIT_OK)
		{
			PrintMessage(ML_INFO, L"Disconnected from target\n");
		}
	}

	// Uninitialise PS3tmapi.
	SNPS3CloseTargetComms();

	// Deallocate target memory.
	FreeTargetList();

	return nReturn;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    SetDefaultOpt
///  @brief     Sets up the default command options.
//////////////////////////////////////////////////////////////////////////////

static void SetDefaultOpt()
{
	// Set the default reset parameters.
	g_TargetOpt.nResetParam = 0;
	g_TargetOpt.nBootParam = SNPS3TM_BOOTP_DEBUG_MODE;
	g_TargetOpt.nBootMask = SNPS3TM_BOOTP_SYSTEM_MODE; // Default to booting into debugger mode

	// Send TTY on given channel.
	g_TargetOpt.nTTYOutChannel = -1;
	g_TargetOpt.pszTTYOutChannel = NULL;

	g_TargetOpt.pTTYImportSource = NULL;

	// Abort handling.
	g_TargetOpt.pszAbortString	= NULL;
	g_TargetOpt.nAbortExitCode	= PS3RUN_EXIT_OK;

	g_TargetOpt.nModulePriority = SNPS3_DEF_PROCESS_PRI;
	g_TargetOpt.nDebugFlags		= 0;
}

//////////////////////////////////////////////////////////////////////////////
///  @anchor    main
///  @brief     Entry point for the application.
//////////////////////////////////////////////////////////////////////////////

int wmain(int argc, WCHAR** argv)
{
	int nRet = PS3RUN_EXIT_ERROR;

	if (argc < 2)
	{
		Usage();
		return 0;
	}

	SetDefaultOpt();

	try
	{
		nRet = ParseCmdLine(argc, argv);
		if (nRet != PS3RUN_EXIT_OK)
			return nRet;

		nRet = ValidateCmdLine();
		if (nRet != PS3RUN_EXIT_OK)
			return nRet;

		nRet = ProcessCmdLine();
		if (nRet != PS3RUN_EXIT_OK)
			return nRet;
	}
	catch (...)
	{
		return PS3RUN_EXIT_ERROR;
	}


	return g_nExitCode;
}
