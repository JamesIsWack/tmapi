//////////////////////////////////////////////////////////////////////////////
///
///	 Copyright (C) 2012, Sony Computer Entertainment Inc. / SN Systems Limited.
///
///  @file      ps3tmapi.h
///  @brief     Target Manager SDK for Sony PlayStation3.
///  @author    SN Systems
///  @date      10/08/12
///
//////////////////////////////////////////////////////////////////////////////

#ifndef PS3TMAPI_H_INC
#define PS3TMAPI_H_INC

#include "APIBase.h" // Include platform generic definitions.
#include "APIUtf8.h"
#include "time.h"

/// \mainpage PS3 TMAPI
/// 
/// The Target Manager (TM) API is used to communicate with development kits by using
/// Target Manager Server.  The TM API attempts to communication with the TM server
/// running in any user session. If no server is running, the TM API 
/// creates a new instance of the server by searching the system search path
/// for the server executable.  
///
/// \section TMAPI Functions limitations
///
///	Using any of the SNPS3xxx functions from within DllMain is dangerous and
/// forbidden. Indeed, the API is multi-threaded, and calling a SNPS3xxx
/// function relies on listening threads and lock mechanism, which is kind of
/// unpredictable when called from DllMain (potential deadlocks).
/// As a general rule, do not do anything TM related from your DllMain!
/// For more details, refer to
/// http://blogs.msdn.com/b/oldnewthing/archive/2007/09/04/4731478.aspx or
/// http://blogs.msdn.com/b/oldnewthing/archive/2004/01/28/63880.aspx

#if defined __cplusplus
extern "C"
{
#endif

#pragma warning(disable : 4200) 	// nonstandard extension used : zero-sized array in struct/union

//////////////////////////////////////////////////////////////////////////////
/// @enum	ESNPS3UNIT
///	@brief	Identifies the different units on the target. Used by any function
///			which specifies a unit.
//////////////////////////////////////////////////////////////////////////////

typedef enum ESNPS3UNIT {
	/// Main PPU unit.
	PS3_UI_CPU = 0,
	/// SPU Unit.
	PS3_UI_SPU,
	/// Raw SPU Unit.
	PS3_UI_SPU_RAW,
	/// Not a type, just used for counts.
	PS3_UI_NUM_TYPES 
} ESNPS3UNIT;

//////////////////////////////////////////////////////////////////////////////
/// @enum	ESNPS3MEMSPACE
///	@brief	Identifies the different memory spaces on the target.
//////////////////////////////////////////////////////////////////////////////

typedef enum ESNPS3MEMSPACE {
	// Main memory space.
	PS3_MEMSPACE_MAIN = 0,
} ESNPS3MEMSPACE;

// @name <LoadFlags>
//@{
//! Load flag.
const UINT32 SNPS3_LOAD_FLAG_ENABLE_DEBUGGING		= 0x00000001;
const UINT32 SNPS3_LOAD_FLAG_USE_ELF_PRIORITY		= 0x00000100;
const UINT32 SNPS3_LOAD_FLAG_USE_ELF_STACKSIZE	= 0x00000200;
const UINT32 SNPS3_LOAD_FLAG_WAIT_BD_MOUNTED		= 0x00002000;
const UINT32 SNPS3_LOAD_FLAG_PPU_NOT_DEBUG		= 0x00010000;
const UINT32 SNPS3_LOAD_FLAG_SPU_NOT_DEBUG		= 0x00020000;
const UINT32 SNPS3_LOAD_FLAG_IGNORE_DEFAULTS		= 0x80000000;
//! Use ELF directory when fileserving PARAM.SFO.
const UINT32 SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USEELFDIR		= 0x00100000;
//! Use custom directory when fileserving PARAM.SFO. Set directory with SNPS3SetCustomParamSFOMappingDirectory().
const UINT32 SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USECUSTOMDIR	= 0x00200000;	
//@}

// @name <ExtraLoadFlags>
//@{
//! Extra load flag (XLF).
const UINT64 SNPS3_XLF_ENABLE_LV2_EXCEPTION_HANDLER		= 0x01;
const UINT64 SNPS3_XLF_ENABLE_REMOTE_PLAY				= 0x02;
const UINT64 SNPS3_XLF_ENABLE_GCM_DEBUG					= 0x04;
const UINT64 SNPS3_XLF_LOAD_LIBPROF_SPRX_AUTOMATICALLY	= 0x08;
const UINT64 SNPS3_XLF_ENABLE_CORE_DUMP					= 0x10;
const UINT64 SNPS3_XLF_ENABLE_ACC_FOR_REMOTE_PLAY		= 0x20;
const UINT64 SNPS3_XLF_ENABLE_HUD_RSX_TOOLS				= 0x40;
const UINT64 SNPS3_XLF_ENABLE_MAT						= 0x80;
const UINT64 SNPS3_XLF_ENABLE_MISC_SETTINGS				= 0x8000000000000000;
const UINT64 SNPS3_XLF_GAME_ATTRIBUTE_INVITE_MESSAGE	= 0x100;
const UINT64 SNPS3_XLF_GAME_ATTRIBUTE_CUSTOM_MESSAGE	= 0x200;
const UINT64 SNPS3_XLF_LOADING_PATCH					= 0x1000;
const UINT64 SNPS3_XLF_ENABLE_M4HD_FOR_REMOTE_PLAY		= 0x2000;
//@}

// @name <ExtraLoadFlagMasks>
//@{
//! Extra load flag (XLF) mask.
const UINT64 SNPS3_XLF_GAME_ATTRIBUTE_MESSAGE_MASK		= 0xF00;
const UINT64 SNPS3_XLF_ALL_FLAG_MASK					= SNPS3_XLF_ENABLE_LV2_EXCEPTION_HANDLER | SNPS3_XLF_ENABLE_REMOTE_PLAY | SNPS3_XLF_ENABLE_GCM_DEBUG |
															SNPS3_XLF_LOAD_LIBPROF_SPRX_AUTOMATICALLY | SNPS3_XLF_ENABLE_CORE_DUMP | SNPS3_XLF_ENABLE_ACC_FOR_REMOTE_PLAY |
SNPS3_XLF_ENABLE_MISC_SETTINGS | SNPS3_XLF_ENABLE_HUD_RSX_TOOLS | SNPS3_XLF_ENABLE_MAT | SNPS3_XLF_GAME_ATTRIBUTE_MESSAGE_MASK |SNPS3_XLF_LOADING_PATCH | SNPS3_XLF_ENABLE_M4HD_FOR_REMOTE_PLAY;
//! Extra load flag (XLF) mask - backward compatibility.
const UINT64 SNPS3_XLF_OVERRIDE_TV_GUI_MASK				= SNPS3_XLF_ENABLE_MISC_SETTINGS;
//@}

// @name <StackSizes>
//@{
//! Stack size value.
const UINT32 SNPS3_ELF_STACK_SIZE_32K		= 0x00000020;		// 32 KBytes.
const UINT32 SNPS3_ELF_STACK_SIZE_64K		= 0x00000040;		// 64 KBytes (default).
const UINT32 SNPS3_ELF_STACK_SIZE_96K		= 0x00000060;		// 96 KBytes.
const UINT32 SNPS3_ELF_STACK_SIZE_128K		= 0x00000080;		// 128 KBytes.
const UINT32 SNPS3_ELF_STACK_SIZE_256K		= 0x00000100;		// 256 KBytes.
const UINT32 SNPS3_ELF_STACK_SIZE_512K		= 0x00000200;		// 512 KBytes.
const UINT32 SNPS3_ELF_STACK_SIZE_1024K		= 0x00000400;		// 1024 KBytes.
const UINT32 SNPS3_ELF_STACK_SIZE_DEFAULT	= SNPS3_ELF_STACK_SIZE_64K;
//@}

// @name <RsxFlags>
//@{
//! RSX flag.
const UINT64 SNPS3_HUD_RSX_USE_RSX_PROFILING_TOOLS = 0x01;
const UINT64 SNPS3_HUD_RSX_USE_FULL_HUD_FEATURES = 0x02;
//@}

// @name <VramFlags>
//@{
//! VRAM flag.
const UINT64 SNPS3_ENABLE_VRAM_CAPTURE = 0x01;
//@}

// @name <SdkVersionMacros>
//@{
//! SDK Version macro.
#define SNPS3_GET_SDK_VER_MAJOR(v)	((v>>16)&0xff)
#define SNPS3_GET_SDK_VER_MINOR(v)	((v>>8)&0xff)
#define SNPS3_GET_SDK_VER_FIX(v)	(v&0xff)
//@}

//////////////////////////////////////////////////////////////////////////////
// PPU Register Definitions.
//////////////////////////////////////////////////////////////////////////////

//! All registers are abstracted by the comms to size SNPS3_REGLEN.
// The user must translate these registers to the correct size.
#define SNPS3_REGLEN			(16)

// @name <PpuGeneralPurposeRegisters>
//@{
//! PPU general purpose register.
#define SNPS3_gpr_0				0x00
#define SNPS3_gpr_1				0x01
#define SNPS3_gpr_2				0x02
#define SNPS3_gpr_3				0x03
#define SNPS3_gpr_4				0x04
#define SNPS3_gpr_5				0x05
#define SNPS3_gpr_6				0x06
#define SNPS3_gpr_7				0x07
#define SNPS3_gpr_8				0x08
#define SNPS3_gpr_9				0x09
#define SNPS3_gpr_10			0x0a
#define SNPS3_gpr_11			0x0b
#define SNPS3_gpr_12			0x0c
#define SNPS3_gpr_13			0x0d
#define SNPS3_gpr_14			0x0e
#define SNPS3_gpr_15			0x0f
#define SNPS3_gpr_16			0x10
#define SNPS3_gpr_17			0x11
#define SNPS3_gpr_18			0x12
#define SNPS3_gpr_19			0x13
#define SNPS3_gpr_20			0x14
#define SNPS3_gpr_21			0x15
#define SNPS3_gpr_22			0x16
#define SNPS3_gpr_23			0x17
#define SNPS3_gpr_24			0x18
#define SNPS3_gpr_25			0x19
#define SNPS3_gpr_26			0x1a
#define SNPS3_gpr_27			0x1b
#define SNPS3_gpr_28			0x1c
#define SNPS3_gpr_29			0x1d
#define SNPS3_gpr_30			0x1e
#define SNPS3_gpr_31			0x1f
//@}

// @name <PpuFloatingPointRegisters>
//@{
//! PPU floating point register.
#define SNPS3_fpr_0				0x20
#define SNPS3_fpr_1				0x21
#define SNPS3_fpr_2				0x22
#define SNPS3_fpr_3				0x23
#define SNPS3_fpr_4				0x24
#define SNPS3_fpr_5				0x25
#define SNPS3_fpr_6				0x26
#define SNPS3_fpr_7				0x27
#define SNPS3_fpr_8				0x28
#define SNPS3_fpr_9				0x29
#define SNPS3_fpr_10			0x2A
#define SNPS3_fpr_11			0x2B
#define SNPS3_fpr_12			0x2C
#define SNPS3_fpr_13			0x2D
#define SNPS3_fpr_14			0x2E
#define SNPS3_fpr_15			0x2F
#define SNPS3_fpr_16			0x30
#define SNPS3_fpr_17			0x31
#define SNPS3_fpr_18			0x32
#define SNPS3_fpr_19			0x33
#define SNPS3_fpr_20			0x34
#define SNPS3_fpr_21			0x35
#define SNPS3_fpr_22			0x36
#define SNPS3_fpr_23			0x37
#define SNPS3_fpr_24			0x38
#define SNPS3_fpr_25			0x39
#define SNPS3_fpr_26			0x3A
#define SNPS3_fpr_27			0x3B
#define SNPS3_fpr_28			0x3C
#define SNPS3_fpr_29			0x3D
#define SNPS3_fpr_30			0x3E
#define SNPS3_fpr_31			0x3F
//@}

// @name <PpuRegisters>
//@{
//! PPU register.
#define SNPS3_pc				0x40
#define SNPS3_cr				0x41
#define SNPS3_lr				0x42
#define SNPS3_ctr				0x43
#define SNPS3_xer				0x44
#define SNPS3_fpscr				0x45
#define SNPS3_vscr				0x46
#define SNPS3_vrsave			0x47
#define SNPS3_msr				0x48

#define SNPS3_vmx_0				0x60
#define SNPS3_vmx_1				0x61
#define SNPS3_vmx_2				0x62
#define SNPS3_vmx_3				0x63
#define SNPS3_vmx_4				0x64
#define SNPS3_vmx_5				0x65
#define SNPS3_vmx_6				0x66
#define SNPS3_vmx_7				0x67
#define SNPS3_vmx_8				0x68
#define SNPS3_vmx_9				0x69
#define SNPS3_vmx_10			0x6A
#define SNPS3_vmx_11			0x6B
#define SNPS3_vmx_12			0x6C
#define SNPS3_vmx_13			0x6D
#define SNPS3_vmx_14			0x6E
#define SNPS3_vmx_15			0x6F
#define SNPS3_vmx_16			0x70
#define SNPS3_vmx_17			0x71
#define SNPS3_vmx_18			0x72
#define SNPS3_vmx_19			0x73
#define SNPS3_vmx_20			0x74
#define SNPS3_vmx_21			0x75
#define SNPS3_vmx_22			0x76
#define SNPS3_vmx_23			0x77
#define SNPS3_vmx_24			0x78
#define SNPS3_vmx_25			0x79
#define SNPS3_vmx_26			0x7A
#define SNPS3_vmx_27			0x7B
#define SNPS3_vmx_28			0x7C
#define SNPS3_vmx_29			0x7D
#define SNPS3_vmx_30			0x7E
#define SNPS3_vmx_31			0x7F
//@}

//////////////////////////////////////////////////////////////////////////////
// SPU Register Definitions
//////////////////////////////////////////////////////////////////////////////

// @name <SpuGeneralPurposeRegisters>
//@{
//! SPU general purpose register.
#define SNPS3_spu_gpr_0			0x00
#define SNPS3_spu_gpr_1			0x01
#define SNPS3_spu_gpr_2			0x02
#define SNPS3_spu_gpr_3			0x03
#define SNPS3_spu_gpr_4			0x04
#define SNPS3_spu_gpr_5			0x05
#define SNPS3_spu_gpr_6			0x06
#define SNPS3_spu_gpr_7			0x07
#define SNPS3_spu_gpr_8			0x08
#define SNPS3_spu_gpr_9			0x09
#define SNPS3_spu_gpr_10		0x0a
#define SNPS3_spu_gpr_11		0x0b
#define SNPS3_spu_gpr_12		0x0c
#define SNPS3_spu_gpr_13		0x0d
#define SNPS3_spu_gpr_14		0x0e
#define SNPS3_spu_gpr_15		0x0f
#define SNPS3_spu_gpr_16		0x10
#define SNPS3_spu_gpr_17		0x11
#define SNPS3_spu_gpr_18		0x12
#define SNPS3_spu_gpr_19		0x13
#define SNPS3_spu_gpr_20		0x14
#define SNPS3_spu_gpr_21		0x15
#define SNPS3_spu_gpr_22		0x16
#define SNPS3_spu_gpr_23		0x17
#define SNPS3_spu_gpr_24		0x18
#define SNPS3_spu_gpr_25		0x19
#define SNPS3_spu_gpr_26		0x1a
#define SNPS3_spu_gpr_27		0x1b
#define SNPS3_spu_gpr_28		0x1c
#define SNPS3_spu_gpr_29		0x1d
#define SNPS3_spu_gpr_30		0x1e
#define SNPS3_spu_gpr_31		0x1f

#define SNPS3_spu_gpr_32		0x20
#define SNPS3_spu_gpr_33		0x21
#define SNPS3_spu_gpr_34		0x22
#define SNPS3_spu_gpr_35		0x23
#define SNPS3_spu_gpr_36		0x24
#define SNPS3_spu_gpr_37		0x25
#define SNPS3_spu_gpr_38		0x26
#define SNPS3_spu_gpr_39		0x27
#define SNPS3_spu_gpr_40		0x28
#define SNPS3_spu_gpr_41		0x29
#define SNPS3_spu_gpr_42		0x2a
#define SNPS3_spu_gpr_43		0x2b
#define SNPS3_spu_gpr_44		0x2c
#define SNPS3_spu_gpr_45		0x2d
#define SNPS3_spu_gpr_46		0x2e
#define SNPS3_spu_gpr_47		0x2f
#define SNPS3_spu_gpr_48		0x30
#define SNPS3_spu_gpr_49		0x31
#define SNPS3_spu_gpr_50		0x32
#define SNPS3_spu_gpr_51		0x33
#define SNPS3_spu_gpr_52		0x34
#define SNPS3_spu_gpr_53		0x35
#define SNPS3_spu_gpr_54		0x36
#define SNPS3_spu_gpr_55		0x37
#define SNPS3_spu_gpr_56		0x38
#define SNPS3_spu_gpr_57		0x39
#define SNPS3_spu_gpr_58		0x3a
#define SNPS3_spu_gpr_59		0x3b
#define SNPS3_spu_gpr_60		0x3c
#define SNPS3_spu_gpr_61		0x3d
#define SNPS3_spu_gpr_62		0x3e
#define SNPS3_spu_gpr_63		0x3f

#define SNPS3_spu_gpr_64		0x40
#define SNPS3_spu_gpr_65		0x41
#define SNPS3_spu_gpr_66		0x42
#define SNPS3_spu_gpr_67		0x43
#define SNPS3_spu_gpr_68		0x44
#define SNPS3_spu_gpr_69		0x45
#define SNPS3_spu_gpr_70		0x46
#define SNPS3_spu_gpr_71		0x47
#define SNPS3_spu_gpr_72		0x48
#define SNPS3_spu_gpr_73		0x49
#define SNPS3_spu_gpr_74		0x4a
#define SNPS3_spu_gpr_75		0x4b
#define SNPS3_spu_gpr_76		0x4c
#define SNPS3_spu_gpr_77		0x4d
#define SNPS3_spu_gpr_78		0x4e
#define SNPS3_spu_gpr_79		0x4f
#define SNPS3_spu_gpr_80		0x50
#define SNPS3_spu_gpr_81		0x51
#define SNPS3_spu_gpr_82		0x52
#define SNPS3_spu_gpr_83		0x53
#define SNPS3_spu_gpr_84		0x54
#define SNPS3_spu_gpr_85		0x55
#define SNPS3_spu_gpr_86		0x56
#define SNPS3_spu_gpr_87		0x57
#define SNPS3_spu_gpr_88		0x58
#define SNPS3_spu_gpr_89		0x59
#define SNPS3_spu_gpr_90		0x5a
#define SNPS3_spu_gpr_91		0x5b
#define SNPS3_spu_gpr_92		0x5c
#define SNPS3_spu_gpr_93		0x5d
#define SNPS3_spu_gpr_94		0x5e
#define SNPS3_spu_gpr_95		0x5f

#define SNPS3_spu_gpr_96		0x60
#define SNPS3_spu_gpr_97		0x61
#define SNPS3_spu_gpr_98		0x62
#define SNPS3_spu_gpr_99		0x63
#define SNPS3_spu_gpr_100		0x64
#define SNPS3_spu_gpr_101		0x65
#define SNPS3_spu_gpr_102		0x66
#define SNPS3_spu_gpr_103		0x67
#define SNPS3_spu_gpr_104		0x68
#define SNPS3_spu_gpr_105		0x69
#define SNPS3_spu_gpr_106		0x6a
#define SNPS3_spu_gpr_107		0x6b
#define SNPS3_spu_gpr_108		0x6c
#define SNPS3_spu_gpr_109		0x6d
#define SNPS3_spu_gpr_110		0x6e
#define SNPS3_spu_gpr_111		0x6f
#define SNPS3_spu_gpr_112		0x70
#define SNPS3_spu_gpr_113		0x71
#define SNPS3_spu_gpr_114		0x72
#define SNPS3_spu_gpr_115		0x73
#define SNPS3_spu_gpr_116		0x74
#define SNPS3_spu_gpr_117		0x75
#define SNPS3_spu_gpr_118		0x76
#define SNPS3_spu_gpr_119		0x77
#define SNPS3_spu_gpr_120		0x78
#define SNPS3_spu_gpr_121		0x79
#define SNPS3_spu_gpr_122		0x7a
#define SNPS3_spu_gpr_123		0x7b
#define SNPS3_spu_gpr_124		0x7c
#define SNPS3_spu_gpr_125		0x7d
#define SNPS3_spu_gpr_126		0x7e
#define SNPS3_spu_gpr_127		0x7f
//@}

// @name <SpuRegisters>
//@{
//! SPU register.
#define SNPS3_spu_pc			0x80
#define SNPS3_spu_fpscr			0x81

#define SNPS3_spu_decrementer	0x90
#define SNPS3_spu_srr0			0x91
#define SNPS3_spu_mb_stat		0x92
#define SNPS3_spu_mb_0			0x93
#define SNPS3_spu_mb_1			0x94
#define SNPS3_spu_mb_2			0x95
#define SNPS3_spu_mb_3			0x96
#define SNPS3_pu_mb				0x97
#define SNPS3_spu_status		0x98
#define SNPS3_spu_cfg			0x99

#define SNPS3_mfc_cq_sr0		0xa0
#define SNPS3_mfc_cq_sr1		0xa1
#define SNPS3_mfc_cq_sr2		0xa2
#define SNPS3_mfc_cq_sr3		0xa3
#define SNPS3_mfc_cq_sr4		0xa4
#define SNPS3_mfc_cq_sr5		0xa5
#define SNPS3_mfc_cq_sr6		0xa6
#define SNPS3_mfc_cq_sr7		0xa7
#define SNPS3_mfc_cq_sr8		0xa8
#define SNPS3_mfc_cq_sr9		0xa9
#define SNPS3_mfc_cq_sr10		0xaa
#define SNPS3_mfc_cq_sr11		0xab
#define SNPS3_mfc_cq_sr12		0xac
#define SNPS3_mfc_cq_sr13		0xad
#define SNPS3_mfc_cq_sr14		0xae
#define SNPS3_mfc_cq_sr15		0xaf
#define SNPS3_mfc_cq_sr16		0xb0
#define SNPS3_mfc_cq_sr17		0xb1
#define SNPS3_mfc_cq_sr18		0xb2
#define SNPS3_mfc_cq_sr19		0xb3
#define SNPS3_mfc_cq_sr20		0xb4
#define SNPS3_mfc_cq_sr21		0xb5
#define SNPS3_mfc_cq_sr22		0xb6
#define SNPS3_mfc_cq_sr23		0xb7
#define SNPS3_mfc_cq_sr24		0xb8
#define SNPS3_mfc_cq_sr25		0xb9
#define SNPS3_mfc_cq_sr26		0xba
#define SNPS3_mfc_cq_sr27		0xbb
#define SNPS3_mfc_cq_sr28		0xbc
#define SNPS3_mfc_cq_sr29		0xbd
#define SNPS3_mfc_cq_sr30		0xbe
#define SNPS3_mfc_cq_sr31		0xbf

#define SNPS3_mfc_cq_sr32		0xc0
#define SNPS3_mfc_cq_sr33		0xc1
#define SNPS3_mfc_cq_sr34		0xc2
#define SNPS3_mfc_cq_sr35		0xc3
#define SNPS3_mfc_cq_sr36		0xc4
#define SNPS3_mfc_cq_sr37		0xc5
#define SNPS3_mfc_cq_sr38		0xc6
#define SNPS3_mfc_cq_sr39		0xc7
#define SNPS3_mfc_cq_sr40		0xc8
#define SNPS3_mfc_cq_sr41		0xc9
#define SNPS3_mfc_cq_sr42		0xca
#define SNPS3_mfc_cq_sr43		0xcb
#define SNPS3_mfc_cq_sr44		0xcc
#define SNPS3_mfc_cq_sr45		0xcd
#define SNPS3_mfc_cq_sr46		0xce
#define SNPS3_mfc_cq_sr47		0xcf
#define SNPS3_mfc_cq_sr48		0xd0
#define SNPS3_mfc_cq_sr49		0xd1
#define SNPS3_mfc_cq_sr50		0xd2
#define SNPS3_mfc_cq_sr51		0xd3
#define SNPS3_mfc_cq_sr52		0xd4
#define SNPS3_mfc_cq_sr53		0xd5
#define SNPS3_mfc_cq_sr54		0xd6
#define SNPS3_mfc_cq_sr55		0xd7
#define SNPS3_mfc_cq_sr56		0xd8
#define SNPS3_mfc_cq_sr57		0xd9
#define SNPS3_mfc_cq_sr58		0xda
#define SNPS3_mfc_cq_sr59		0xdb
#define SNPS3_mfc_cq_sr60		0xdc
#define SNPS3_mfc_cq_sr61		0xdd
#define SNPS3_mfc_cq_sr62		0xde
#define SNPS3_mfc_cq_sr63		0xdf

#define SNPS3_mfc_cq_sr64		0xe0
#define SNPS3_mfc_cq_sr65		0xe1
#define SNPS3_mfc_cq_sr66		0xe2
#define SNPS3_mfc_cq_sr67		0xe3
#define SNPS3_mfc_cq_sr68		0xe4
#define SNPS3_mfc_cq_sr69		0xe5
#define SNPS3_mfc_cq_sr70		0xe6
#define SNPS3_mfc_cq_sr71		0xe7
#define SNPS3_mfc_cq_sr72		0xe8
#define SNPS3_mfc_cq_sr73		0xe9
#define SNPS3_mfc_cq_sr74		0xea
#define SNPS3_mfc_cq_sr75		0xeb
#define SNPS3_mfc_cq_sr76		0xec
#define SNPS3_mfc_cq_sr77		0xed
#define SNPS3_mfc_cq_sr78		0xee
#define SNPS3_mfc_cq_sr79		0xef
#define SNPS3_mfc_cq_sr80		0xf0
#define SNPS3_mfc_cq_sr81		0xf1
#define SNPS3_mfc_cq_sr82		0xf2
#define SNPS3_mfc_cq_sr83		0xf3
#define SNPS3_mfc_cq_sr84		0xf4
#define SNPS3_mfc_cq_sr85		0xf5
#define SNPS3_mfc_cq_sr86		0xf6
#define SNPS3_mfc_cq_sr87		0xf7
#define SNPS3_mfc_cq_sr88		0xf8
#define SNPS3_mfc_cq_sr89		0xf9
#define SNPS3_mfc_cq_sr90		0xfa
#define SNPS3_mfc_cq_sr91		0xfb
#define SNPS3_mfc_cq_sr92		0xfc
#define SNPS3_mfc_cq_sr93		0xfd
#define SNPS3_mfc_cq_sr94		0xfe
#define SNPS3_mfc_cq_sr95		0xff
//@}

//////////////////////////////////////////////////////////////////////////////
// PLAYSTATION(R)3 Target Events.
//////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_DBG_EVENT_HDR
///	@brief		Header for debug events.
///	@details	When a target event of type SN_TGT_EVENT_TARGET_SPECIFIC occurs,
///             the event header \link Apibase::SN_EVENT_TARGET_HDR SN_EVENT_TARGET_HDR\endlink will be followed by an
///             \link Apibase::SNPS3_DBG_EVENT_HDR SNPS3_DBG_EVENT_HDR\endlink 
///             struct, followed by an \link Apibase::SNPS3_DBG_EVENT_DATA SNPS3_DBG_EVENT_DATA\endlink.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_DBG_EVENT_HDR {
	/// Command ID - Always equals fixed value DBGP_EVENT_NOTIFICATION - see DBGP specification if more details are required.
	UINT32	uCommandID;
	/// Request ID from DBGP header, should be ignored, see DBGP specification if if more details are required.
	UINT32	uRequestID;
	/// Length of data in bytes.
	UINT32	uDataLength;
	/// Process ID where relevant to event, see DBGP specification if if more details are required.
	UINT32	uProcessID;
	/// Result code from DBGP header, always zero, should be ignored, see DBGP specification if if more details are required.
	UINT32	uResult;
};
typedef struct SNPS3_DBG_EVENT_HDR SNPS3_DBG_EVENT_HDR;

// @name <TargetSpecificEvents>
//@{
//! Notification that a process was created.
#define SNPS3_DBG_EVENT_PROCESS_CREATE				(0x00000000)
//! Notification that a process was exited.
#define SNPS3_DBG_EVENT_PROCESS_EXIT				(0x00000001)
//! Notification that a process was forcibly killed.
#define SNPS3_DBG_EVENT_PROCESS_KILL				(0x00000002)
//! Notification that a game process has exited via a call to exitspawn().
#define SNPS3_DBG_EVENT_PROCESS_EXITSPAWN			(0x00000003)

//! Notification that a program interrupt occurred because a trap instruction was executed.
#define SNPS3_DBG_EVENT_PPU_EXP_TRAP				(0x00000010)
//! Notification that a program interrupt occurred because a privilege instruction was executed.
#define SNPS3_DBG_EVENT_PPU_EXP_PREV_INT			(0x00000011)
//! Notification that an alignment interrupt occurred.
#define SNPS3_DBG_EVENT_PPU_EXP_ALIGNMENT			(0x00000012)
//! Notification that a program interrupt occurred because an illegal instruction was executed.
#define SNPS3_DBG_EVENT_PPU_EXP_ILL_INST			(0x00000013)
//! Notification that an instruction storage interrupt occurred.
#define SNPS3_DBG_EVENT_PPU_EXP_TEXT_HTAB_MISS		(0x00000014)
//! Notification that an instruction segment interrupt occurred.
#define SNPS3_DBG_EVENT_PPU_EXP_TEXT_SLB_MISS		(0x00000015)
//! Notification that a data storage interrupt occurred.
#define SNPS3_DBG_EVENT_PPU_EXP_DATA_HTAB_MISS		(0x00000016)
//! Notification that a program interrupt occurred because of a floating point enabled exception.
#define SNPS3_DBG_EVENT_PPU_EXP_FLOAT				(0x00000017)
//! Notification that a data segment interrupt occurred.
#define SNPS3_DBG_EVENT_PPU_EXP_DATA_SLB_MISS		(0x00000018)
//! Notification that an address set as DABR (Data Address Breakpoint Register) was accessed.
#define SNPS3_DBG_EVENT_PPU_EXP_DABR_MATCH			(0x00000019)
//! Notification that a PPU thread was stopped by DBGP_STOP_PPU_THREAD.
#define SNPS3_DBG_EVENT_PPU_EXP_STOP				(0x0000001a)
//! Notification that a primary PPU thread was stopped at entry point after process was created.
#define SNPS3_DBG_EVENT_PPU_EXP_STOP_INIT			(0x0000001b)
//! Notification that a memory access trap interrupt occurred.
#define SNPS3_DBG_EVENT_PPU_EXC_DATA_MAT			(0x0000001c)

//! Notification that a PPU thread was created.
#define SNPS3_DBG_EVENT_PPU_THREAD_CREATE			(0x00000020)
//! Notification that a PPU thread was exited.
#define SNPS3_DBG_EVENT_PPU_THREAD_EXIT				(0x00000021)

//! Notification that an SPU thread was started.
#define SNPS3_DBG_EVENT_SPU_THREAD_START			(0x00000030)
//! Notification that an SPU thread was stopped by an exception or DBGP_STOP_SPU_THREAD_GROUP.
#define SNPS3_DBG_EVENT_SPU_THREAD_STOP				(0x00000031)
//! Notification that an SPU thread was stopped at entry point after an SPU thread group was started.
#define SNPS3_DBG_EVENT_SPU_THREAD_STOP_INIT		(0x00000032)
//! Notification that an SPU thread group was destroyed.
#define SNPS3_DBG_EVENT_SPU_THREAD_GROUP_DESTROY	(0x00000033)
//! Notification that an SPU thread was stopped by an exception or DBGP_STOP_SPU_THREAD_GROUP.
//! This event is triggered in addition to DBG_EVENT_SPU_THREAD_STOP when the cause of the SPU
//! exception is SYS_SPU_EXCEPTION_MFC_SEGMENT, SYS_SPU_EXCEPTION_MFC_STORAGE or
//! SYS_SPU_EXCEPTION_MAT, such as Cause = 0x00000010, 0x00000020 or 0x00002000.
#define SNPS3_DBG_EVENT_SPU_THREAD_STOP_EX			(0x00000034)

#define SNPS3_DBG_EVENT_RAW_NOTIFY					(0xf000000f)

//! Notification that a PRX was loaded.
#define SNPS3_DBG_EVENT_PRX_LOAD					(0x00000040)
//! Notification that a PRX module was unloaded.
#define SNPS3_DBG_EVENT_PRX_UNLOAD					(0x00000041)

//! Notification that the debug agent initialized.
#define SNPS3_DBG_EVENT_DA_INITIALIZED				(0x00000060)

//! Notification that an event was received from the footswitch.
#define SNPS3_DBG_EVENT_FOOTSWITCH_EVENT		    (0x00000070)

// ! Notification that a package has been installed. Contains installed path. See SNPS3InstallPackage()
#define SNPS3_DBG_EVENT_INSTALL_PACKAGE_PATH		(0x00000081)

/// Due to internal SDK limitation, these events are only triggered
/// if coredump is generated in app_home
//! Notification that a core dump has been completed.
#define SNPS3_DBG_EVENT_CORE_DUMP_COMPLETE			(0x00000100)
//! Notification that a core dump has been started.
#define SNPS3_DBG_EVENT_CORE_DUMP_START				(0x00000101)

//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_PROCESS_CREATE_DATA
///	@brief		Data associated with process create event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_PROCESS_CREATE_DATA {
	UINT32			uParentProcessID;
	///  UTF-8 null-terminated file name string.
	UCHAR			szFilename[];
};
typedef struct SNPS3_PPU_PROCESS_CREATE_DATA SNPS3_PPU_PROCESS_CREATE_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_PROCESS_EXIT_DATA
///	@brief		Data associated with process exit event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_PROCESS_EXIT_DATA {
	UINT64			uExitCode;
};
typedef struct SNPS3_PPU_PROCESS_EXIT_DATA SNPS3_PPU_PROCESS_EXIT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_PROCESS_EXITSPAWN_DATA
///	@brief		Data associated with process exitspawn event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_PROCESS_EXITSPAWN_DATA {
	/// sys_dbg_process_wait_for_game_process() status if ret == success, else ret value.
	UINT64			uStatus;
	/// Exited process ID is in the header.
	UINT32			uNewProcessID;
};
typedef struct SNPS3_PPU_PROCESS_EXITSPAWN_DATA SNPS3_PPU_PROCESS_EXITSPAWN_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_TRAP_DATA
///	@brief		Data associated with ppu trap event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_TRAP_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_TRAP_DATA SNPS3_PPU_EXP_TRAP_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_PREV_INT_DATA
///	@brief		Data associated with privileged instruction event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_PREV_INT_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_PREV_INT_DATA SNPS3_PPU_EXP_PREV_INT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_ALIGNMENT_DATA
///	@brief		Data associated with alignment error event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_ALIGNMENT_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uDSISR;
	UINT64			uDAR;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_ALIGNMENT_DATA SNPS3_PPU_EXP_ALIGNMENT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_DATA_MAT_DATA
///	@brief		Data associated with memory access trap event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_DATA_MAT_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uDSISR;
	UINT64			uDAR;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_DATA_MAT_DATA SNPS3_PPU_EXP_DATA_MAT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_ILL_INST_DATA
///	@brief		Data associated with illegal instruction event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_ILL_INST_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uDSISR;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_ILL_INST_DATA SNPS3_PPU_EXP_ILL_INST_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_TEXT_HTAB_MISS_DATA
///	@brief		Data associated with htab miss event (reading instructions).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_TEXT_HTAB_MISS_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_TEXT_HTAB_MISS_DATA SNPS3_PPU_EXP_TEXT_HTAB_MISS_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_TEXT_SLB_MISS_DATA
///	@brief		Data associated with slb miss event (reading instructions).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_TEXT_SLB_MISS_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_TEXT_SLB_MISS_DATA SNPS3_PPU_EXP_TEXT_SLB_MISS_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_DATA_HTAB_MISS_DATA
///	@brief		Data associated with htab miss event (reading data).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_DATA_HTAB_MISS_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_DATA_HTAB_MISS_DATA SNPS3_PPU_EXP_DATA_HTAB_MISS_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_FLOAT_DATA
///	@brief		Data associated with floating point exception event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_FLOAT_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_FLOAT_DATA SNPS3_PPU_EXP_FLOAT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_DATA_SLB_MISS_DATA
///	@brief		Data associated with slb miss event (reading data).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_DATA_SLB_MISS_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_DATA_SLB_MISS_DATA SNPS3_PPU_EXP_DATA_SLB_MISS_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_DABR_MATCH_DATA
///	@brief		Data associated with data access breakpoint register event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_DABR_MATCH_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_DABR_MATCH_DATA SNPS3_PPU_EXP_DABR_MATCH_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_STOP_DATA
///	@brief		Data associated with ppu stop event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_STOP_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_STOP_DATA SNPS3_PPU_EXP_STOP_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_EXP_STOP_INIT_DATA
///	@brief		Data associated with ppu stop init event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_EXP_STOP_INIT_DATA {
	UINT64			uPPUThreadID;
	UINT32			uHWThreadNumber;
	UINT64			uPC;
	UINT64			uSP;
};
typedef struct SNPS3_PPU_EXP_STOP_INIT_DATA SNPS3_PPU_EXP_STOP_INIT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_THREAD_CREATE_DATA
///	@brief		Data associated with ppu thread create event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_THREAD_CREATE_DATA { 
	UINT64			uPPUThreadID;
};
typedef struct SNPS3_PPU_THREAD_CREATE_DATA SNPS3_PPU_THREAD_CREATE_DATA;


//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_PPU_THREAD_EXIT_DATA
///	@brief		Data associated with ppu thread exit event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_THREAD_EXIT_DATA {
	UINT64			uPPUThreadID;
};
typedef struct SNPS3_PPU_THREAD_EXIT_DATA SNPS3_PPU_THREAD_EXIT_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_SPU_THREAD_START_DATA
///	@brief		Data associated with spu threadgroup start event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_SPU_THREAD_START_DATA {
	UINT32			uSPUThreadGroupID;
	UINT32			uSPUThreadID;
	/// UTF-8 filename of SPU ELF loaded onto thread.
	UCHAR			aData[];
};
typedef struct SNPS3_SPU_THREAD_START_DATA SNPS3_SPU_THREAD_START_DATA;

// @name <SpuThreadStopReason>
//@{
//! No exception.
#define SNPS3_SPU_EXP_NO_EXCEPTION			(0x00000000)
//! DMA alignment.
#define SNPS3_SPU_EXP_DMA_ALIGNMENT			(0x00000001)
//! DMA command.
#define SNPS3_SPU_EXP_DMA_COMMAND			(0x00000002)
//! Error.
#define SNPS3_SPU_EXP_ERROR					(0x00000004)
//! MFC FIR.
#define SNPS3_SPU_EXP_MFC_FIR				(0x00000008)
//! MFC segment.
#define SNPS3_SPU_EXP_MFC_SEGMENT			(0x00000010)
//! MFC storage.
#define SNPS3_SPU_EXP_MFC_STORAGE			(0x00000020)
//! No value.
#define SNPS3_SPU_EXP_NO_VALUE				(0x00000040)
//! Stop call.
#define SNPS3_SPU_EXP_STOP_CALL				(0x00000100)
//! Stop call.
#define SNPS3_SPU_EXP_STOPD_CALL			(0x00000200)
//! Halt.
#define SNPS3_SPU_EXP_HALT					(0x00000400)
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_SPU_THREAD_STOP_DATA
///	@brief		Data associated with spu thread stop event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_SPU_THREAD_STOP_DATA {
	UINT32			uSPUThreadGroupID;
	UINT32			uSPUThreadID;
	UINT32			uPC;
	UINT32			uReason;
	UINT32			uSP;
};
typedef struct SNPS3_SPU_THREAD_STOP_DATA SNPS3_SPU_THREAD_STOP_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_SPU_THREAD_STOP_EX_DATA
///	@brief		Extended data associated with spu thread stop events MFC_STORAGE, 
///             MFC_SEGMENT and MAT access.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_SPU_THREAD_STOP_EX_DATA {
	UINT32			uSPUThreadGroupID;
	UINT32			uSPUThreadID;
	UINT32			uPC;
	UINT32			uReason;
	UINT32			uSP;
	UINT64			uMfc_dsisr;
	UINT64			uMfc_dsipr;
	UINT64			uMfc_dar;
};

//////////////////////////////////////////////////////////////////////////////
/// @deprecated Use	SNPS3_SPU_THREAD_STOP_EX_DATA.
/// @brief Use SNPS3_SPU_THREAD_STOP_EX_DATA.
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3_SPU_THREAD_STOP_EX_DATA instead")
struct SNPS3_SPU_THREAD_STOP_DATA_EX
{
	UINT32			uSPUThreadGroupID;
	UINT32			uSPUThreadID;
	UINT32			uPC;
	UINT32			uReason;
	UINT32			uSP;
	UINT64			uMfc_dsisr;
	UINT64			uMfc_dar;
	UINT64			uMfc_dsipr;	
};

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_SPU_THREAD_GROUP_DESTROY_DATA
///	@brief		Data associated with spu thread group destroy event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_SPU_THREAD_GROUP_DESTROY_DATA {
	UINT32			uSPUThreadGroupID;
};
typedef struct SNPS3_SPU_THREAD_GROUP_DESTROY_DATA SNPS3_SPU_THREAD_GROUP_DESTROY_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_EVENTNOTIFY_PRX_LOAD_DATA
///	@brief		Data associated with PRX load event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_EVENTNOTIFY_PRX_LOAD_DATA {
	UINT64			uPPUThreadID;
	UINT32			uPRXID;
	UINT64			uTimestamp;
};
typedef struct SNPS3_EVENTNOTIFY_PRX_LOAD_DATA SNPS3_EVENTNOTIFY_PRX_LOAD_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_EVENTNOTIFY_PRX_UNLOAD_DATA
///	@brief		Data associated with PRX unload event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_EVENTNOTIFY_PRX_UNLOAD_DATA {
	UINT64			uPPUThreadID;
	UINT32			uPRXID;
	UINT64			uTimestamp;
};
typedef struct SNPS3_EVENTNOTIFY_PRX_UNLOAD_DATA SNPS3_EVENTNOTIFY_PRX_UNLOAD_DATA;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_EVENTNOTIFY_FOOTSWITCH
///	@brief		Data associated with footswitch event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_EVENTNOTIFY_FOOTSWITCH
{
	UINT64			uEventSource;
	UINT64			uEventData1;
	UINT64			uEventData2;
	UINT64			uEventData3;
	UINT64			uReserved;
};
typedef struct SNPS3_EVENTNOTIFY_FOOTSWITCH SNPS3_EVENTNOTIFY_FOOTSWITCH;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_EVENTNOTIFY_INSTALL_PACKAGE_PATH
///	@brief		Data associated with install package path event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_EVENTNOTIFY_INSTALL_PACKAGE_PATH
{
	///  UTF-8 null-terminated string.
	char			path[1024];
};
typedef struct SNPS3_EVENTNOTIFY_INSTALL_PACKAGE_PATH SNPS3_EVENTNOTIFY_INSTALL_PACKAGE_PATH;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_EVENTNOTIFY_CORE_DUMP_COMPLETE
///	@brief		Data associated with core dump complete event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_EVENTNOTIFY_CORE_DUMP_COMPLETE
{
	///  UTF-8 null-terminated string.
	char			filename[1024];
};

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_EVENTNOTIFY_CORE_DUMP_START
///	@brief		Data associated with core dump start event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_EVENTNOTIFY_CORE_DUMP_START
{
	///  UTF-8 null-terminated string.
	char			filename[1024];
};

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_DBG_EVENT_DATA
///	@brief		Overall data structure for debug events.
///	@details	When a target event of type SN_TGT_EVENT_TARGET_SPECIFIC occurs,
///             the event header \link Apibase::SN_EVENT_TARGET_HDR SN_EVENT_TARGET_HDR\endlink will be followed by an
///             \link Apibase::SNPS3_DBG_EVENT_HDR SNPS3_DBG_EVENT_HDR\endlink 
///             struct, followed by an \link Apibase::SNPS3_DBG_EVENT_DATA SNPS3_DBG_EVENT_DATA\endlink  
///             whose uEventType member will be one of \link SNPS3_DBG_EVENT_PROCESS_CREATE SNPS3_DBG_EVENT_PROCESS_CREATE\endlink
///             etc. Data should be interpreted according to this value.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_DBG_EVENT_DATA 
{
	UINT32			uEventType;

	union {
		SNPS3_PPU_PROCESS_CREATE_DATA				ppu_process_create;
		SNPS3_PPU_PROCESS_EXIT_DATA					ppu_process_exit;
		SNPS3_PPU_PROCESS_EXITSPAWN_DATA			ppu_process_exitspawn;
		SNPS3_PPU_EXP_TRAP_DATA						ppu_exc_trap;
		SNPS3_PPU_EXP_PREV_INT_DATA					ppu_exc_prev_int;
		SNPS3_PPU_EXP_ALIGNMENT_DATA				ppu_exc_alignment;
		SNPS3_PPU_EXP_ILL_INST_DATA					ppu_exc_ill_inst;
		SNPS3_PPU_EXP_TEXT_HTAB_MISS_DATA			ppu_exc_text_htab_miss;
		SNPS3_PPU_EXP_TEXT_SLB_MISS_DATA			ppu_exc_text_slb_miss;
		SNPS3_PPU_EXP_DATA_HTAB_MISS_DATA			ppu_exc_data_htab_miss;
		SNPS3_PPU_EXP_FLOAT_DATA					ppu_exc_float;
		SNPS3_PPU_EXP_DATA_SLB_MISS_DATA			ppu_exc_data_slb_miss;
		SNPS3_PPU_EXP_DABR_MATCH_DATA				ppu_exc_dabr_match;
		SNPS3_PPU_EXP_STOP_DATA						ppu_exc_stop;
		SNPS3_PPU_EXP_STOP_INIT_DATA				ppu_exc_stop_init;
		SNPS3_PPU_EXP_DATA_MAT_DATA					ppu_exc_data_mat;

		SNPS3_PPU_THREAD_CREATE_DATA				ppu_thread_create;
		SNPS3_PPU_THREAD_EXIT_DATA					ppu_thread_exit;

		SNPS3_SPU_THREAD_START_DATA					spu_thread_start;
		SNPS3_SPU_THREAD_STOP_DATA					spu_thread_stop;
		SNPS3_SPU_THREAD_STOP_EX_DATA				spu_thread_stop_ex;
		SNPS3_SPU_THREAD_STOP_DATA					spu_thread_stop_init;
		SNPS3_SPU_THREAD_GROUP_DESTROY_DATA			spu_thread_group_destroy;

		SNPS3_EVENTNOTIFY_PRX_LOAD_DATA				prx_load;
		SNPS3_EVENTNOTIFY_PRX_UNLOAD_DATA			prx_unload;

		SNPS3_EVENTNOTIFY_FOOTSWITCH				footswitch_data;

		SNPS3_EVENTNOTIFY_INSTALL_PACKAGE_PATH		install_package_path;

		SNPS3_EVENTNOTIFY_CORE_DUMP_START			core_dump_start;
		SNPS3_EVENTNOTIFY_CORE_DUMP_COMPLETE		core_dump_complete;
	};
};
typedef struct SNPS3_DBG_EVENT_DATA SNPS3_DBG_EVENT_DATA;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3TargetInfo
///	@brief		Contains parameters which define a target
///             (see SNPS3GetTargetInfo() and SNPS3SetTargetInfo()).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3TargetInfo {
	/// Flags indicating what values returned/to be set (see defines SN_TI_TARGETID to SN_TI_BOOT).
	UINT32        nFlags;
	/// Handle to target.
	HTARGET     hTarget;
	/// Target name. UTF-8 null-terminated string.
	const char  *pszName;
	/// Target type. UTF-8 null-terminated string.
	const char	*pszType;
	/// Target information. UTF-8 null-terminated string.
	const char  *pszInfo;
	/// Home directory of target. UTF-8 null-terminated string.
	const char  *pszHomeDir;
	/// File serving directory of target. UTF-8 null-terminated string.
	const char  *pszFSDir;
	/// Boot flags for target.
	UINT64      boot;
};
typedef struct SNPS3TargetInfo SNPS3TargetInfo;

// @name <SystemInfoFlags>
//@{
//! System information flag.
#define SYS_INFO_SDK_VERSION       (0x00000001)
#define SYS_INFO_TIMEBASE_FREQ     (0x00000002)
#define SYS_INFO_RT_SDK_VERSION    (0x00000004)
#define SYS_INFO_TOTAL_SYSTEM_MEM  (0x00000008)
#define SYS_INFO_AVAILABLE_SYS_MEM (0x00000010)
#define SYS_INFO_DCM_BUFFER_SIZE   (0x00000020)
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3SystemInfo
///	@brief		Contains miscellaneous information about the system. (see SNPS3GetSystemInfo()).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3SystemInfo
{
	/// SDK version used to build debug agent.
	UINT32			uCellSdkVersion;
	/// The timebase frequency of the target.
	UINT64			uTimebaseFrequency;
	/// The runtime version of the target SDK.
	UINT32			uCellRuntimeSdkVersion;
	/// The total system memory of the target.
	UINT32			uTotalSystemMemory;
	/// The available system memory.
	UINT32			uAvailableSystemMemory;
	/// Max size of DBGP buffer	.
	UINT32			uDCMBufferSize;
};
typedef struct SNPS3SystemInfo SNPS3SystemInfo;

//! Maximum length of a directory entry name.
#define SNPS3_DIRENT_NAME_LEN	(256)

// @name <DirectoryEntryTypes>
//@{
//! Directory entry type.
#define SNPS3_DIRENT_TYPE_UNKNOWN	(0)
#define SNPS3_DIRENT_TYPE_DIRECTORY	(1)
#define SNPS3_DIRENT_TYPE_REGULAR	(2)
#define SNPS3_DIRENT_TYPE_SYMLINK	(3)
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3DirEntry
///	@brief	Contains parameters which define a directory entry 
///			(see SNPS3GetDirectoryList()).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3DirEntry
{
	/// The file type. See SNPS3_DIRENT_TYPE_* defines	
	UINT32 Type;
	/// File access attributes in standard octal format.
	UINT32 Mode;
	/// The last access time of the file.
	struct tm AccessTime;
	/// The modified time of the file.
	struct tm ModifiedTime;
	/// The create time of the file.
	struct tm CreateTime;
	/// The file size.
	UINT64 Size;
	/// The file name. UTF-8 null-terminated string.
	char Name[SNPS3_DIRENT_NAME_LEN];
};
typedef struct SNPS3DirEntry SNPS3DirEntry;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3TargetTimezone
///	@brief	Contains parameters which define the target timezone and
///         dst settings
//////////////////////////////////////////////////////////////////////////////

struct SNPS3TargetTimezone
{
	/// The timezone settings (offset from gmt, in seconds)	
	INT32 Timezone;
	/// The daylight saving time settings (in seconds)
	INT32 Dst;
};
typedef struct SNPS3TargetTimezone SNPS3TargetTimezone;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3DirEntryEx
///	@brief	Contains parameters which define a directory entry 
///			(see SNPS3GetDirectoryListEx()).
///          File timestamp is returned in time_t UTC format.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3DirEntryEx
{
	/// The file type. See SNPS3_DIRENT_TYPE_* defines	
	UINT32 Type;
	/// File access attributes in standard octal format.
	UINT32 Mode;
	/// The last access time of the file.
	UINT64 AccessTimeUTC;
	/// The modified time of the file.
	UINT64 ModifiedTimeUTC;
	/// The create time of the file.
	UINT64 CreateTimeUTC;
	/// The file size.
	UINT64 Size;
	/// The file name. UTF-8 null-terminated string.
	char Name[SNPS3_DIRENT_NAME_LEN];
};
typedef struct SNPS3DirEntryEx SNPS3DirEntryEx;

//! Maximum path length on PS3.
#define SNPS3_MAX_TARGET_PATH (1024 + 32)

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3Transfer
///	@brief	File transfer information (see SNPS3GetFileTransferInfo(), 
///			SNPS3GetFileTransferList()).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3Transfer
{
	/// Status ID's.
	enum STATUS {
		PENDING		= (1<<0),
		FAILED		= (1<<1),
		SUCCEEDED	= (1<<2),
		SKIPPED		= (1<<3),
		IN_PROGRESS	= (1<<4),
		CANCELLED	= (1<<5)
	};

	/// Transfer ID.
	UINT32 ID;
	/// Status of transfer.
	UINT32 Status;
	/// Source path. UTF-8 null-terminated string.
	char Source[MAX_PATH];
	/// Destination path. UTF-8 null-terminated string.
	char Destination[SNPS3_MAX_TARGET_PATH];
	/// Total size of file in bytes.
	UINT64 Size;
	/// Total number of bytes read.
	UINT64 Read;
};
typedef struct SNPS3Transfer SNPS3Transfer;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Returns a textual description of an SN error code.
///
///  @param     snr              SN error code.
///  @param     ppszErrorString  Pointer to receive error string. UTF-8 null-terminated string.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Error string found and translated.\n
///             SN_E_BAD_PARAM - ppszErrorString was NULL pointer.\n
///             SN_E_ERROR - snr was an invalid code.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3TranslateError(SNRESULT snr, const char **ppszErrorString);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Initialises target communications and launches Target Manager.
///  This function must be called by each application thread before functions in 
///  the DLL can be used by that thread. It sets up any necessary data structures and establishes 
///  communication with the Target Manager.\n
///  Note that because this function internally starts thread(s), it should not be called inside
///  DllMain() / DLL_PROCESS_ATTACH, or deadlock can occur.
///  When the application has finished using the DLL, SNPS3CloseTargetComms() should be called.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Communications initialised successfully.\n
///             SN_S_TM_VERSION - Warning: Different version of Target Manager.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_ERROR - Internal error.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3InitTargetComms(void);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Shuts down internal communications (but does not close the 
///             Target Manager) and frees resources.
///             Note that because this function internally stops thread(s), it
///             should not be called inside DllMain() / DLL_PROCESS_DETACH, or
///             deadlock can occur.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Communications closed.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3CloseTargetComms(void);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     List available target types.
///             Call this function with pTypes = NULL to get the number of types.
///             To get the target types, set pTypes to point to an allocated memory block of size 
///             (*puNumTypes * sizeof(\link TMAPI_TARGET_TYPE TMAPI_TARGET_TYPE\endlink)),
///             and call the function again.\n
///
///  @param     puNumTypes      Pointer to unsigned int containing number of
///                             instances of \link TMAPI_TARGET_TYPE TMAPI_TARGET_TYPE\endlink to go in pTypes buffer 
///                             (the size of which should be this value * sizeof(\link TMAPI_TARGET_TYPE TMAPI_TARGET_TYPE\endlink)).
///  @param     pTypes			Pointer to buffer to receive list of target
///                             \link TMAPI_TARGET_TYPE TMAPI_TARGET_TYPE\endlink structures. If this 
///                             parameter is NULL then the value puNumTypes 
///                             points to is set to the no. of types available
///                             and SN_S_OK is returned.
///                             If puNumTypes is less than the no. of types
///                             available then the value it points to is set to 
///                             the no. of types available and SN_S_OUT_OF_MEM is 
///                             returned.
///
///  @return    \link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved target types or number of types.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() SNPS3InitTargetComms() or SNPS3InitTargetComms first.\n
///             SN_E_BAD_PARAM - puNumTypes was NULL pointer.\n
///             SN_E_OUT_OF_MEM - There are more types available than specified in puNumTypes.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ListTargetTypes(UINT32 *puNumTypes, TMAPI_TARGET_TYPE *pTypes);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Add a new target to the Target Manager.
///             Creates a new PS3 target based on the parameters supplied.
///             If the target already exists with different type/parameters the 
///             function will return SN_E_BAD_TARGET.
///
///  @param     pszName         Pointer to zero terminated UTF-8 char array containing
///                             target name (for example "ps3devtool").
///  @param     pszType			Pointer to zero terminated char array containing
///                             target type (one of the types returned by
///                             \link SNPS3ListTargetTypes() SNPS3ListTargetTypes()\endlink).
///  @param     uConnParamSize	Size of connect parameter in bytes (for IP
///                             targets this is sizeof(\link TMAPI_TCPIP_CONNECT_PROP TMAPI_TCPIP_CONNECT_PROP\endlink)).
///  @param		pConnParams		Pointer to connection parameters block, for
///                             IP targets this is an instance of \link TMAPI_TCPIP_CONNECT_PROP TMAPI_TCPIP_CONNECT_PROP\endlink.
///                             If this is NULL then default connection parameters are used.
///  @param     pnTarget        Pointer to handle to receive target ID.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Target added successfully.\n
///             SN_S_NO_ACTION - Target with same name, same type and same connect parameters already exists.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - Target already exists, or invalid target type.\n
///             SN_E_BAD_PARAM - pnTarget is NULL pointer, or pConnParams is NULL pointer and uConnParamSize is non-zero.
/// 
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3AddTarget(const char *pszName, const char *pszType, UINT32 uConnParamSize,
							  BYTE *pConnParams, HTARGET *pnTarget);

//! Invalid target handle value.
const HTARGET SNPS3_INVALID_TARGET = 0xffffffff;

//! Default target special handle value.
const HTARGET SNPS3_DEFAULT_TARGET = 0xfffffffe;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets the default target.\n
///				To set the default target, see SNPS3SetDefaultTarget().
///				If there is no default target, the function returns SN_E_BAD_TARGET, and writes
///				SNPS3_INVALID_TARGET to pTarget.
///				This should ONLY happen if TM Server has no targets.
///
///  @param     pTarget        Default target ID.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///				SN_S_OK - Successfully retrieved default target.\n
///				SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///				SN_E_BAD_TARGET - No default target currently set.\n
///				SN_E_BAD_PARAM - pnTarget is NULL pointer.
/// 
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDefaultTarget(HTARGET *pTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets the default target.\n
///
///  @param     pTarget        Default target ID.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///				SN_S_OK - Successfully set default target.\n
///				SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///				SN_E_BAD_TARGET - hTarget was invalid.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetDefaultTarget(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Delete a specified target from the Target Manager.
///
///             Warning - this operation is irreversible.
///
///  @param     hTarget           Handle to valid target.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully deleted target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3DeleteTarget(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated  Use SNPS3EnumerateTargets().
///  @brief  Use SNPS3EnumerateTargets().
///
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3EnumerateTargets instead")
SNAPI SNRESULT SNPS3EnumTargets(TMAPI_EnumTargetsCB pfnCallBack);

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated  Use SNPS3EnumerateTargetsEx().
///  @brief  Use SNPS3EnumerateTargetsEx().
///
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3EnumerateTargetsEx instead")
SNAPI SNRESULT SNPS3EnumTargetsEx(TMAPI_EnumTargetsExCB pfnCallBack, void *pArg);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Displays a dialog centered in the window passed or to the 
///             desktop if NULL is passed in which displays a list of the 
///             targets available in the Target Manager.
///             The user can choose a target which is returned through the 
///             buffer passed in.\n
///             If no targets are available, the dialog will not be displayed 
///             and the function will return SN_E_NO_TARGETS.\n
///             If the user presses cancel or does not select a target before
///             clicking OK, the function will return SN_E_NO_SEL.\n
///             If the function succeeds, the function will return SN_S_OK 
///             and the contents of the buffer pointed to by phTarget will be 
///             set to the handle for the selected target.\n
///             This handle can then be used in subsequent operations.
///
///  @param     hWndOwner    Handle to owner window or NULL for desktop.
///  @param     phTarget     Pointer to buffer to receive target handle.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Target chosen successfully.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - phTarget was NULL pointer.\n
///             SN_E_NO_TARGETS - No targets available.\n
///             SN_E_OUT_OF_MEM - Not enough memory available for target picker.\n
///             SN_E_NO_SEL - User did not select a target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3PickTarget(HWND hWndOwner, HTARGET *phTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Connect to specified target.
///
///  @param     hTarget           Handle to valid target.
///  @param     pszApplication    Name of connecting application.
///                               Reserved for future use, should be NULL.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully connected to target.\n
///             SN_S_NO_ACTION - Target already connected.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Connect(HTARGET hTarget, const char *pszApplication);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Connect to specified target.
///
///  @param     hTarget           Handle to valid target.
///  @param     pszApplication    Name of connecting application.
///                               Reserved for future use, should be NULL.
///  @param     bForceFlag        Set to true to force disconnect an already connected user.
///                               If false, then same behaviour as SNPS3Connect()
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully connected to target.\n
///             SN_S_NO_ACTION - Target already connected.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ConnectEx(HTARGET hTarget, const char *pszApplication, BOOL bForceFlag);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Disconnect from specified target.
///
///  @param     hTarget    Handle to valid target.
///
///  @result	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully disconnected from target.\n
///             SN_S_NO_ACTION - Not connected to target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Disconnect(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Forcibly disconnect another user from the specified target.
///
///  @param     hTarget    Handle to valid target.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully disconnected another user.\n
///             SN_S_NO_ACTION - Currently connected - can not forcibly disconnect oneself. Call SNPS3Disconnect() for this purpose.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ForceDisconnect(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Returns the connection status of the specified target.
///             If this function succeeds it returns SN_S_OK and 
///             *pnConnectStatus contains the connection status of the target.\n
///             If *pnConnectStatus is \link ApiBase::CS_IN_USE CS_IN_USE\endlink, then ppszUsage will point to 
///             a string containing details of the current user. Otherwise, ppszUsage will point to
///             a string signifying that the target is not in use. The content 
///             of the string returned is temporary and must be copied to a 
///             permanent location.
///
///  @param     hTarget            Handle to target.
///  @param     puConnectStatus    Pointer to variable for connect status.
///                                (See \link ApiBase::ECONNECTSTATUS ECONNECTSTATUS\endlink).
///  @param     ppszUsage          Returns a pointer to the UTF-8 null-terminated usage string.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved connect status.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puConnectStatus or ppszUsage is NULL.\n
///             SN_E_OUT_OF_MEM - Not enough memory available for temporary string.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetConnectStatus(HTARGET hTarget, ECONNECTSTATUS *puConnectStatus, char **ppszUsage);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     List all available TTY streams for a particular target.
///             To obtain a list of TTY Streams, call this function passing 
///             NULL for pStreams to get the number of TTY streams. Allocate 
///             a block of \link TTYSTREAM TTYSTREAM\endlink structures and call this function again 
///             passing in a pointer to this buffer.
///
///  @param     hTarget            Handle to target.
///  @param     puNumTTYStreams    [in] Size of buffer in number of streams.\n
///                                [out] Number of streams for which space is required, 
///                                or if buffer is big enough, number written to buffer.
///  @param     pStreams           Pointer to buffer to receive array of 
///                                stream info structures.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved streams list or number of streams.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puNumTTYStreams is a NULL pointer.\n
///             SN_E_OUT_OF_MEM - Number of streams in puNumTTYStreams is less than actual number of streams.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ListTTYStreams(HTARGET hTarget, UINT32 *puNumTTYStreams, TTYSTREAM *pStreams);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Kick the event notification queue. When an application registers one or more callbacks,
///             this function must be called to retrieve events and facilitate the calling of callbacks.
///				Version 370+ has changed the way it operates in a multi-threaded environment.
///				Calling SNPS3Kick() in a specific thread will now only kick the callbacks that were
///				registered in that thread.
///				This means it is now safe to register callbacks in any thread that
///				needs to process notifications, as long as SNPS3Kick() is called in each thread.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - One or more events were processed.\n
///             SN_S_NO_MSG - There were no messages to process.\n
///             SN_E_TM_NOT_RUNNING - Server was shutdown during function call.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Kick();

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated  Use SNPS3RegisterTTYEventHandler()
///  @brief  Use SNPS3RegisterTTYEventHandler()
///
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3RegisterTTYEventHandler instead")
SNAPI SNRESULT SNPS3RegisterTTYEvents(HTARGET hTarget, UINT32 uStream, TMAPI_HandleEventCB pfnCallBack, void *pUserData);

//! Specifies that all available TTY streams are to be acted on, rather than an individual stream.
#define SNPS3_TTY_ALL_STREAMS					(UINT32) -1

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Register a callback function which will be called when TTY 
///             output is available from a target. 
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param		hTarget			Handle to a target.
///  @param     uStream			TTY stream to receive TTY events on or SNPS3_TTY_ALL_STREAMS for all streams.\n
///                             If SNPS3_TTY_ALL_STREAMS is specified and an error occurs (other than SN_E_BAD_PARAM or SN_E_DLL_NOT_INITIALISED), 
///                             all streams will be unregistered.
///  @param     pfnCallBack		Pointer to callback.
///  @param		pUserData		Optional user defined data.
///
///  Callback must be in the form of:
///
/// \code
///  __stdcall void Callback(
///          HTARGET hTarget,       // Handle to target.
///          UINT32 Type,           // SN_EVENT_TTY.
///          UINT32 Param,          // Stream ID.
///          SNRESULT eResult,      // Success or failure of data fetch.
///          UINT32 Length,         // Length of data in event including header.
///          BYTE* Data,            // Pointer to event data.
///          void* UserData);       // Optional user data.
/// \endcode
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered for target events.\n
///             SN_S_REPLACED - Already registered for TTY events, previous callback function has been replaced.\n
///             In the case of SNPS3_TTY_ALL_STREAMS, this signifies a callback for at least one stream was replaced.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Pointer to callback function was null.\n
///             SN_E_ERROR - Internal error.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterTTYEventHandler(HTARGET hTarget, UINT32 uStream, 
											TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Unregister the current callback function from receiving TTY.
///             If \link PS3TMAPI::SNPS3_TTY_ALL_STREAMS SNPS3_TTY_ALL_STREAMS\endlink is specified and errors occur unregistering
///             one or more streams, the returned error code is that of the last failure.
///
///  @param     hTarget	   Handle to target.
///  @param     uStream	   Stream ID previously registered or \link PS3TMAPI.h::SNPS3_TTY_ALL_STREAMS SNPS3_TTY_ALL_STREAMS\endlink for all streams.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered from TTY events.\n
///             SN_S_NO_ACTION - No callback function registered to that stream.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3CancelTTYEvents(HTARGET hTarget, UINT32 uStream);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Send TTY data to the specified TTY stream.
///
///  @param     hTarget	   Handle to target.
///  @param     uStream	   Stream ID.
///  @param     pszText    Text.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - TTY data sent successfully.\n
///             SN_E_BAD_PARAM - pszText was NULL pointer.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SendTTY(HTARGET hTarget, UINT32 uStream, const char *pszText);

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated  Use SNPS3RegisterTargetEventHandler()
///  @brief  Use SNPS3RegisterTargetEventHandler()
///
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3RegisterTargetEventHandler instead")
SNAPI SNRESULT SNPS3RegisterTargetEvents(HTARGET hTarget,
										 TMAPI_HandleEventCB pfnCallBack, void *pUserData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Unregister the current callback function from receiving events.
///
///  @param     hTarget	   Handle to target.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Target events cancelled successfully.\n
///             SN_S_NO_ACTION - No callback function registered.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3CancelTargetEvents(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Retrieves information for a target specified by hTarget 
///             member of SNPS3TargetInfo() structure.
///             SN_TI_TARGETID flag in nFlags member *must* be set. 
///             Members are set when structure returns and setting of 
///             other flags has no effect. The boot parameter is not retrieved (on return, see the nFlags
///             member to check which members have been set).
///             Contents of structure returned are temporary and must be copied to a permanent location.
///
///  @param     pTargetInfo    Address of target info structure.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved target information.\n
///             SN_E_BAD_PARAM - Pointer to structure is NULL or SN_TI_TARGETID not set.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetTargetInfo(SNPS3TargetInfo *pTargetInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Returns the version of the API the code was built against.
///
///  @param     ppszVersion    Pointer to a const char* to receive the 
///                            version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved API version.\n
///             SN_E_BAD_PARAM - ppszVersion was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetAPIVersion(char **ppszVersion);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Returns the version of the Target Manager currently connected.
///
///  @param     ppszVersion    Pointer to a const char* to receive the 
///                             version. The content pointed to is temporary and must be copied to a 
///                             permanent location.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved Target Manager version.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_PARAM - ppszVersion was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetTMVersion(char **ppszVersion);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets target information based on members of supplied
///             SNPS3TargetInfo() structure.
///             Only members identified by nFlags member will be set. The 
///             desired target to change is indicated by the hTarget member 
///             for which nFlags *must* include the SN_TI_TARGETID flag.\n
///             The info field (SN_TI_INFO) and boot parameter field (SN_TI_BOOT)
///             cannot be set. See SNPS3SetBootParameter() and SNPS3SetConnectionInfo() to set these.\n
///             Target notifications (see SNPS3RegisterTargetEventHandler()) are sent for each field 
///             changed. See \link ApiBase::SN_TGT_EVENT_DETAILS SN_TGT_EVENT_DETAILS\endlink.
///
///  @param     pTargetInfo    Address of target info structure.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set target information.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pTargetInfo was NULL, SN_TI_TARGETID not in flags, SN_TI_INFO or SN_TI_BOOT in flags.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetTargetInfo(SNPS3TargetInfo *pTargetInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Takes a string containing the name of a target (for example 
///             "ps3devtool") and fills the supplied handle with the 
///             corresponding target ID.
///
///  @param     pszTgtName    UTF-8 null-terminated target name string.
///  @param     phTarget      Pointer to buffer to receive target handle.
///
///  @return    \link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved target ID.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - Target does not exist, or pszTgtName was NULL.\n
///             SN_E_NO_TARGETS - No targets available.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetTargetFromName(const char *pszTgtName, HTARGET *phTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Returns the number of targets that are currently set up in Target Manager.
///
///  @param     puNumTargets    Pointer to unsigned int to receive number of targets.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved number of targets.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_PARAM - puNumTargets was NULL.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetNumTargets(UINT32 *puNumTargets);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Retrieves the current status of the specified processor unit.
///
///  @param     hTarget         Handle to target.
///  @param     uUnit           Processor unit (see \link PS3TMAPI::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param     pnUnitStatus    Pointer to variable for status. See \link ApiBase.h::EUNITSTATUS EUNITSTATUS\endlink.
///                             
///  @param     pnReasonCode    Not used for PS3, should be NULL.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved unit status.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pnUnitStatus was NULL pointer.\n
///             SN_E_BAD_UNIT - uUnit was an invalid unit ID.
///             
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetStatus(HTARGET hTarget, UINT32 uUnit, long *pnUnitStatus, long *pnReasonCode);

// @name <ResetParameters>
//@{
//! Perform a soft reset (default).
const UINT64 SNPS3TM_RESETP_SOFT_RESET				= 0x0000000000000000ui64;
//! Perform a hard reset (default = soft).
const UINT64 SNPS3TM_RESETP_HARD_RESET				= 0x0000000000000001ui64;
//! Perform a quick reset.
const UINT64 SNPS3TM_RESETP_QUICK_RESET				= 0x0000000000000002ui64;
//! Do not display reset settings in tty - Do not set this bit in the mask to change current reset only
//! Setting this bit in the mask will change TM (persisted) settings
const UINT64 SNPS3TM_RESETP_DONT_DISPLAY_SETTINGS	= 0x2000000000000000ui64;
//! Perform a reset using separate off/on commands (all other flags are honoured).
const UINT64 SNPS3TM_RESETP_RESET_EX				= 0x8000000000000000ui64;	
//@}

// @name <ResetParameterMask>
//@{
//! Reset parameter mode mask.
const UINT64 SNPS3TM_RESETP_SOFT_HARD_MASK			= 0x0000000000000001ui64;
//! Reset parameter type mask.
const UINT64 SNPS3TM_RESETP_QUICK_FULL_MASK			= 0x0000000000000002ui64;
//! Reset parameter ex mask.
const UINT64 SNPS3TM_RESETP_RESET_EX_MASK			= 0x8000000000000000ui64;
//! Reset parameter all mask.
const UINT64 SNPS3TM_RESETP_ALL_MASK				= SNPS3TM_RESETP_SOFT_HARD_MASK | SNPS3TM_RESETP_QUICK_FULL_MASK | SNPS3TM_RESETP_RESET_EX_MASK;
//@}

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Resets specified target allowing specification of reset parameters.
///
///  @param     hTarget     Handle to a valid target.
///  @param     uReset	    PS3 reset parameter - See SNPS3TM_RESETP_SOFT_RESET etc.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully reset target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Reset(HTARGET hTarget, UINT64 uReset);

//! Process priority.
#define SNPS3_DEF_PROCESS_PRI    (0x3e9)

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Loads an ELF file, returning the process ID and thread ID for the process.
///
///  @param     hTarget			Handle to target.
///  @param		uPriority		Priority to be given to the primary PPU thread (for example SNPS3_DEF_PROCESS_PRI).
///  @param		pszFilename		UTF-8 null-terminated filename of ELF to download. /app_home/ will automatically be added to the start of this path.
///                             The maximum size of the path is 1024 bytes.
///								To load from dev_hdd0, the path should start with "../dev_hdd0/" in order to bypass automatic addition of /app_home/.
///  @param		argc			Size of list of arguments to be passed to main function.
///  @param		argv			Argument list to be passed to main function. Total max size is 4096 bytes.
///  @param		envc			Size of list of environment variables.
///  @param		envv			List of environment variables to be set for the process in format: VAR=value.
///                             Total max size os 4096 bytes.
///  @param		puProcessID		Pointer to buffer to receive process ID (or NULL).
///  @param		puThreadID		Pointer to buffer to receive thread ID (or NULL).
///  @param		uDebugFlags		Debug flags. See SNPS3_LOAD_FLAG_*
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.6.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Process successfully loaded.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM		- Either pszFilename was NULL pointer or argv or envv were NULL whilst argc or envc were non-zero.\n
///             SN_E_DATA_TOO_LONG	- One or more of pszFilename, argv or envv exceeded max size allowed by protocol.\n
///             SN_E_DECI_ERROR     - PARAM.SFO was required and not found.\n
///             SN_E_TM_COMMS_ERR   - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR      - Error launching the process - check tty for more information. Common errors are:\n
///                             		0x80010002 - The path exceeds max size allowed by device.\n
///                             		0x80010006 - File not found.				
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessLoad(HTARGET hTarget, UINT32 uPriority, const char *pszFilename, 
								int argc, const char** argv, int envc, const char** envv,
								UINT32 *puProcessID, UINT64 *puThreadID, UINT32 uDebugFlags);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Fetches a list of processes running on the specified target.
///             Call this function with puBuffer = NULL to get the number of processes in puCount.
///             To get the process list, set puBuffer to point to a memory block of size (*puCount * sizeof(UINT32)),
///             and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param		puCount			[in] Maximum number of IDs buffer can hold.\n
///                             [out] Number of processes for which space is required, 
///                             or if buffer is big enough, number written to buffer.
///  @param		puBuffer		Pointer to a buffer to receive the information.
///                             As a guide, allocate a buffer of 64 * sizeof(UINT32),
///                             passing 64 as the count.
/// 
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved process list or number of processes.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount (in UINT32s) smaller than required.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessList(HTARGET hTarget, UINT32 *puCount, UINT32 *puBuffer);

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3MODULEINFO_HDR
///	@brief	PRX module info.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MODULEINFO_HDR {
	/// Module name.
	char	aName[30];
	/// Module version.
	char	aVersion[2];
	/// Module attributes.
	UINT32	uAttribute;
	/// Address of start entry.
	UINT32	uStartEntry;
	/// Address of stop entry.
	UINT32	uStopEntry;
	/// Full filename of PRX module.
	char	aElfName[512];
	/// Number of segments in module.
	UINT32	uNumSegments;
};
typedef struct SNPS3MODULEINFO_HDR SNPS3MODULEINFO_HDR;
typedef SNPS3MODULEINFO_HDR *PSNPS3MODULEINFO_HDR;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3PRXSEGMENT
///	@brief	Information about segments within PRX module.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PRXSEGMENT {
	UINT64	uBase;
	UINT64	uFileSize;
	UINT64	uMemSize;
	UINT64	uIndex;
	UINT64	uElfType;
};
typedef struct SNPS3PRXSEGMENT SNPS3PRXSEGMENT;
typedef SNPS3PRXSEGMENT *PSNPS3PPRXSEGMENT;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3PRXSEGMENTEX
///	@brief	Extended information about segment within PRX module.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PRXSEGMENTEX {
	UINT64	uBase;
	UINT64	uFileSize;
	UINT64	uMemSize;
	UINT64	uIndex;
	UINT64	uElfType;
	UINT64	uFlags;
	UINT64	uAlign;
};
typedef struct SNPS3PRXSEGMENTEX SNPS3PRXSEGMENTEX;
typedef SNPS3PRXSEGMENTEX *SNPS3PPRXSEGMENTEX;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3MODULEINFO
///	@brief	Contains parameters which define a module (see SNPS3GetModuleInfo()).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MODULEINFO {
	SNPS3MODULEINFO_HDR Hdr;
	SNPS3PRXSEGMENT Segments[];
};
typedef struct SNPS3MODULEINFO SNPS3MODULEINFO;
typedef SNPS3MODULEINFO *PSNPS3MODULEINFO;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Fetches module information.
///             To get the module information, first call this function with pModuleInfo = NULL to get the required size.
///             Allocate memory of size of the value pointed to by the returned puBufferSize, set pModuleInfo to point
///             to this memory and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		ID of the process.
///  @param     uModuleID		ID of the module.
///  @param		puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pModuleInfo		Pointer to a buffer to hold the module info.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved module information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer defined in puBufferSize was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetModuleInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uModuleID, UINT64 *puBufferSize, SNPS3MODULEINFO *pModuleInfo);

//////////////////////////////////////////////////////////////////////////////
///	@brief Extended module information
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MODULEINFOEX {
	SNPS3MODULEINFO_HDR Hdr;
	SNPS3PRXSEGMENTEX Segments[];
};
typedef struct SNPS3MODULEINFOEX SNPS3MODULEINFOEX;
typedef SNPS3MODULEINFOEX *PSNPS3MODULEINFOEX;

//////////////////////////////////////////////////////////////////////////////
///	@brief	MSELF information
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MSELFINFO
{
	UINT64 uMselfFileOffset;
	UCHAR  aReserved[32];
};
typedef struct SNPS3MSELFINFO SNPS3MSELFINFO;

//////////////////////////////////////////////////////////////////////////////
///	@brief	Extra module information contains GUID of PRX module loaded 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EXTRAMODULEINFO
{
	UINT32 uaPPU_GUID[5];
};
typedef struct SNPS3EXTRAMODULEINFO SNPS3EXTRAMODULEINFO;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Fetches module information.
///             To get the module information, first call this function with pModuleInfo = NULL to get the required size.
///             Allocate memory of size of the value pointed to by the returned puBufferSize, set pModuleInfo to point
///             to this memory and call the function again.
///
///  @param     hTarget				Handle to target
///  @param		uProcessID			ID of the process.
///  @param     uModuleID			ID of the module.
///  @param		puBufferSize		[in] Size of the buffer.\n
///                             	[out] Required size of buffer, or if buffer is big enough, bytes written to buffer.\n
///                             	NOTE: The buffer size holds all module information plus extra MSELF information.
///  @param		pModuleInfo			Pointer to a buffer to hold the module info.
///  @param		ppMSELFInfo			Address of a pointer which will hold the address in buffer of the MSELF information. Can be NULL.
///	 @param		pExtraModuleInfo	Pointer to an instance of \link SNPS3EXTRAMODULEINFO SNPS3EXTRAMODULEINFO\endlink to hold extra info (if set to NULL it is ignored).
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved module information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize smaller than required buffer size
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetModuleInfoEx(HTARGET hTarget, UINT32 uProcessID, UINT32 uModuleID, UINT64 *puBufferSize, 
									SNPS3MODULEINFOEX *pModuleInfo, SNPS3MSELFINFO** ppMSELFInfo, SNPS3EXTRAMODULEINFO *pExtraModuleInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Fetches a list of modules loaded by a given process.
///             Call this function with puBuffer = NULL to get the number of modules.
///             To get the module list, set puBuffer to point to a memory block of size *puCount * sizeof(UINT32),
///             and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		ID of the process to which the module belongs.
///  @param     puCount			[in] Number of module IDs buffer can hold.\n
///                             [out] Number of modules for which space is required, 
///                             or if buffer is big enough, number written to buffer.
///  @param		puBuffer		Pointer to a buffer to receive the information.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved module list or number of modules.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Buffer too small (size signified by (*puCount * sizeof(UINT32))).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetModuleList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *puBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets the connection information for a given target.
///
///  @param     hTarget			Handle to target.
///  @param     pConnection		Pointer to a buffer to receive the 
///                             \link TMAPI_TCPIP_CONNECT_PROP TMAPI_TCPIP_CONNECT_PROP\endlink structure.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved connection information.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pConnection was NULL pointer.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetConnectionInfo(HTARGET hTarget, TMAPI_TCPIP_CONNECT_PROP *pConnection);

//////////////////////////////////////////////////////////////////////////////
/// @enum	ESNPS3PROCESSSTATUS
///	@brief	This enumerated type defines the different states that a process can be in. 
//////////////////////////////////////////////////////////////////////////////

enum ESNPS3PROCESSSTATUS {
	/// Process is being created
	ESNPS3_PS_CREATING = 1,  
	/// Process is ready (a PU thread exists in the process)
	ESNPS3_PS_READY = 2,  
	/// The process has exited but has not reported the exit status yet
	ESNPS3_PS_EXITED = 3     
};

#pragma pack(push, 1)

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3PROCESSINFO_HDR
///	@brief	Process information header	 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PROCESSINFO_HDR {
	/// Process status (see \link PS3tmapi.h::ESNPS3PROCESSSTATUS ESNPS3PROCESSSTATUS\endlink).
	UINT32	uStatus;			
	/// Number of PPU threads.
	UINT32	uNumPPUThreads;		
	/// Number of SPU threads.
	UINT32	uNumSPUThreads;		
	/// Parent process ID.
	UINT32	uParentProcessID;	
	/// Maximum memory size for process.
	UINT64	uMaxMemorySize;		
	/// Path to the loaded elf. UTF-8 null-terminated string.
	char	szPath[512];		
};
typedef struct SNPS3PROCESSINFO_HDR SNPS3PROCESSINFO_HDR;
typedef SNPS3PROCESSINFO_HDR *PSNPS3PROCESSINFO_HDR;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3PROCESSINFO
///	@brief	This structure contains the parameters that define a process. It is used in the function
///			SNPS3ProcessInfo to obtain information about a particular process.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PROCESSINFO {
	SNPS3PROCESSINFO_HDR Hdr;
	UINT64 ThreadIDs[];
};
typedef struct SNPS3PROCESSINFO SNPS3PROCESSINFO;
typedef SNPS3PROCESSINFO *PSNPS3PROCESSINFO;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3EXTRAPROCESSINFO
///	@brief	Extra process information (GUID of elf)	 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EXTRAPROCESSINFO
{
	UINT32 uaPPU_GUID[5];
};
typedef struct SNPS3EXTRAPROCESSINFO SNPS3EXTRAPROCESSINFO;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3PROCESSLOADPARAMS
///	@brief	Debug parameters passed during load.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PROCESSLOADPARAMS
{
	UINT64 uVersion;
	UINT64 uData[16];
};
typedef struct SNPS3PROCESSLOADPARAMS SNPS3PROCESSLOADPARAMS;

/// @brief Flag set if information in structure is valid.
#define SNPS3_PROCESSLOADINFO_FLAGS_INFO_VALID			0x01

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3PROCESSLOADINFO
///	@brief	Information on debug state of program. Populated by SNPS3ProcessInfoEx2().
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PROCESSLOADINFO
{
	/// On return from SNPS3ProcessInfoEx2(), value is SNPS3_PROCESSLOADINFO_FLAGS_INFO_VALID if loaded by the debug agent 
	/// (thus the rest of this information will be correct), 0 otherwise.
	UINT32 uInfoValid;			
	/// Load flags passed into the load command
	UINT32 uDebugFlags;		
	/// Parameters passed during load
	SNPS3PROCESSLOADPARAMS LoadInfo;
};

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Fetches process information for a given process ID.
///             Call this function with pProcessInfo = NULL to get the required buffer size.
///             To get the process information, set pProcessInfo to point to an allocated memory block of size *puBufferSize,
///             and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///  @param		puBufferSize	[in] Size of buffer.
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pProcessInfo    Buffer for returned process information. This
///                             consists of a header \link SNPS3PROCESSINFO_HDR SNPS3PROCESSINFO_HDR\endlink followed
///                             by the thread IDs for PPU and SPU respectively.
///                             Each thread ID is 64 bits long.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved process information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of pProcessInfo buffer specified in puBufferSize was smaller than required.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target, or invalid process ID supplied.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 *puBufferSize, SNPS3PROCESSINFO *pProcessInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets extended information for a specified process.
///             Call this function with pProcessInfo = NULL to get the required buffer size.
///             To get the process information, set pProcessInfo to point to an allocated memory block of size *puBufferSize,
///             and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///  @param		puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pProcessInfo    Buffer for returned process information. This
///                             consists of a header \link SNPS3PROCESSINFO_HDR SNPS3PROCESSINFO_HDR\endlink followed
///                             by the thread IDs for PPU and SPU respectively.
///                             Each thread ID is 64 bits long.
///	 @param		pExtra			Pointer to instance of \link SNPS3EXTRAPROCESSINFO SNPS3EXTRAPROCESSINFO\endlink structure to hold data.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved process information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize or pExtra was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of pProcessInfo buffer given in puBufferSize was smaller than required buffer.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessInfoEx(HTARGET hTarget, UINT32 uProcessID, UINT32 *puBufferSize, SNPS3PROCESSINFO *pProcessInfo, 
	SNPS3EXTRAPROCESSINFO *pExtra);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets further extended information for a specified process.
///             Call this function with pProcessInfo = NULL to get the required buffer size.
///             To get the process information, set pProcessInfo to point to an allocated memory block of size *puBufferSize,
///             and call the function again.
///
///  @param     hTarget			Handle to target
///  @param		uProcessID		Process ID.
///  @param		puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pProcessInfo    Buffer for returned process information. This
///                             consists of a header \link SNPS3PROCESSINFO_HDR SNPS3PROCESSINFO_HDR\endlink followed
///                             by the thread IDs for PPU and SPU respectively.
///                             Each thread ID is 64 bits long.
///	 @param		pExtra			Pointer to instance of \link SNPS3EXTRAPROCESSINFO SNPS3EXTRAPROCESSINFO\endlink structure to hold data.
///	 @param		pDebug			Optional pointer to instance of \link SNPS3PROCESSLOADINFO SNPS3PROCESSLOADINFO\endlink structure to hold data.
///                             Can be NULL.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved process information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize or pExtra was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of pProcessInfo buffer given in puBufferSize was smaller than required.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessInfoEx2(HTARGET hTarget, UINT32 uProcessID, UINT32 *puBufferSize, SNPS3PROCESSINFO *pProcessInfo,
								   SNPS3EXTRAPROCESSINFO *pExtra, SNPS3PROCESSLOADINFO *pDebug);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Continues all threads from a specified process.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully continued threads.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_ERROR - Internal error.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessContinue(HTARGET hTarget, UINT32 uProcessID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Stops all threads from a specified process.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
/// 
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully stopped threads.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_ERROR - Internal error.\n
///             SN_E_COMMS_ERR - Error communicating with target.
/// 
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessStop(HTARGET hTarget, UINT32 uProcessID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Kills all threads of a specified process.
///             The threads are deleted and cannot be restarted.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully killed process.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_ERROR - Internal error.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessKill(HTARGET hTarget, UINT32 uProcessID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets a range of memory for the specified processor unit on the target specified by
///             hTarget.
///
///  @param     hTarget		Handle to target.
///  @param     uUnit		Processor unit (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param		uProcessID	Process ID.
///  @param		uThreadID	Thread ID.\n
///             			For PPU, thread ID ignored.\n
///             			For SPU, specify the thread group ID.
///  @param     uAddress	Address to read from.
///  @param     nCount		Count in bytes to read.
///  @param     pBuffer		Pointer to buffer for memory.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved process memory.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Either pBuffer is NULL pointer or process ID / thread group ID (if on SPU) was SYS_PID_INVALID (defined in SDK process.h).\n
///             SN_E_BAD_UNIT - Bad unit value (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink for valid values).\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_ERROR - Internal error.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessGetMemory(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID, 
									 UINT64 uAddress, int nCount, BYTE *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets a range of memory for the specified processor unit.
///
///  @param     hTarget		Handle to target.
///  @param     uUnit		Processor unit (see enum \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param		uProcessID	Process ID.
///  @param		uThreadID	Thread ID.
///             			- For PPU, specify -1.
///             			- For SPU, specify the thread group ID.
///  @param     uAddress	Address to write to.
///  @param     nCount		Number of bytes to write.
///  @param     pBuffer		Pointer to buffer containing data to write.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set process memory.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pBuffer NULL pointer or process ID / thread ID (on SPU) was SYS_PID_INVALID (defined in SDK process.h).\n
///             SN_E_BAD_UNIT - uUnit was invalid unit value (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessSetMemory(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID, 
									 UINT64 uAddress, int nCount, const BYTE *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Returns a list of threads from a specified process.
///             Call this function with pPPUThreadIDs and pSPUThreadGroupIDs both = NULL to get the required buffer sizes.
///             To get the thread list, set pPPUThreadIDs to point to an allocated memory block of size (*puNumPPUThreads * sizeof(UINT64)),
///             set pSPUThreadGroupIDs to point to an allocated memory block of size (*puNumSPUThreadGroups * sizeof(UINT32))
///             and call the function again.
///
///  @param     hTarget     Handle to a valid target.
///	 @param		uProcessID	Process ID.
///	 @param		puNumPPUThreads
///             			[in] Number of thread IDs buffer pPPUThreadIDs points to can hold.\n
///             			[out] Number of thread IDs written to buffer pPPUThreadIDs points to,
///             			or number of thread IDs available (when pPPUThreadIDs was set to null).
///	 @param     pPPUThreadIDs
///             			Pointer to a buffer to receive the list of thread ids.\n
///                         If this parameter is NULL, the function will write the number of IDs the 
///             			buffer needs to hold to puNumPPUThreads.
///  @param		puNumSPUThreadGroups
///             			[in] Number of thread IDs buffer pSPUThreadGroupIDs points to can hold.\n
///             			[out] Number of thread IDs written to buffer pSPUThreadIDs points to.
///             			or number of thread IDs available (when pSPUThreadIDs was set to null).
///	 @param     pSPUThreadGroupIDs
///             			Pointer to a buffer to receive the list of thread ids.
///                         If this parameter is NULL, the function will write the number of IDs the 
///             			buffer needs to hold to puNumSPUThreadGroups.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved thread list or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puNumPPUThreads or puNumSPUThreads was NULL pointer or uProcessID invalid value.\n
///             SN_E_OUT_OF_MEM - Number of IDs either pPPUThreadIDs or pSPUThreadGroupIDs can hold 
///             (as specified in puNumPPUThreads or puNumSPUThreadGroups) is too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puNumPPUThreads, UINT64 *pPPUThreadIDs,
							   UINT32 *puNumSPUThreadGroups, UINT64 *pSPUThreadGroupIDs);

// @name <SpuThreadGroupState>
//@{
//! SPU thread group state.
#define SNPS3_SPU_NOT_CONFIGURED	0x00000000
#define SNPS3_SPU_CONFIGURED		0x00000001
#define SNPS3_SPU_READY				0x00000002
#define SNPS3_SPU_WAITING			0x00000003
#define SNPS3_SPU_SUSPENDED			0x00000004
#define SNPS3_SPU_WAITINGSUSPENDED	0x00000005
#define SNPS3_SPU_RUNNING			0x00000006
#define SNPS3_SPU_STOPPED			0x00000007
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct SNPS3_SPU_THREADGROUP_INFO
///	@brief	SPU threadgroup information. 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_SPU_THREADGROUP_INFO
{
	/// SPU thread group ID.
	UINT32	uThreadGroupID;			
	/// State (see defines SNPS3_SPU_NOT_CONFIGURED to SNPS3_SPU_STOPPED).
	UINT32	uState;					
	/// Priority of thread group.
	UINT32	uPriority;				
	/// Number of threads in group.
	UINT32	uNumThreads;			
	/// Length of thread group name (including NULL terminator).
	UINT32	uThreadGroupNameLen;	
};
typedef struct SNPS3_SPU_THREADGROUP_INFO SNPS3_SPU_THREADGROUP_INFO;
typedef SNPS3_SPU_THREADGROUP_INFO *PSNPS3_SPU_THREADGROUP_INFO;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about an SPU thread group.
///             Call this function with pBuffer = NULL to get the required buffer size.
///             To get the thread group information, set pBuffer to point to an allocated memory block of size *puBufferSize,
///             and call the function again.\n
///             On success, the buffer contains instance of \link SNPS3_SPU_THREADGROUP_INFO SNPS3_SPU_THREADGROUP_INFO\endlink, followed by an array
///             of 32 bit SPU thread IDs in the group and finally the threadgroup name.
///
///  @param     hTarget				Handle to a valid target.
///	 @param		uProcessID			Process ID.
///	 @param		uThreadGroupID		64-bit thread group ID.
///  @param		puBufferSize		[in] Size of pBuffer.\n
///                             	[out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pBuffer				Pointer to a buffer to receive the information. 
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved SPU thread group information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was a NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer (specified in puBufferSize) too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSPUThreadGroupInfo(HTARGET hTarget, UINT32 uProcessID, UINT64 uThreadGroupID,
										  UINT32 *puBufferSize ,BYTE *pBuffer);

// @name <PpuThreadState>
//@{
//! PPU thread state.
#define SNPS3_PPU_IDLE				0x00000000
#define SNPS3_PPU_RUNNABLE			0x00000001
#define SNPS3_PPU_ONPROC			0x00000002
#define SNPS3_PPU_SLEEP				0x00000003
#define SNPS3_PPU_SUSPENDED			0x00000004
#define SNPS3_PPU_SLEEP_SUSPENDED	0x00000005
#define SNPS3_PPU_STOP				0x00000006
#define SNPS3_PPU_ZOMBIE			0x00000007
#define SNPS3_PPU_DELETED			0x00000008
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3_PPU_THREAD_INFO	
/// @brief	This structure contains the parameters that define a PPU thread. It is used in the
///			function SNPS3ThreadInfo() to obtain information about a particular thread.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_THREAD_INFO
{
	/// Thread ID.
	UINT64	uThreadID;			
	/// Current priority.
	UINT32	uPriority;			
	/// Current state (see defines SNPS3_PPU_IDLE to SNPS3_PPU_DELETED).
	UINT32	uState;				
	/// Start address of stack.
	UINT64	uStackAddress;		
	/// Stack size (in bytes).
	UINT64	uStackSize;			
	/// Length of thread name (including NULL terminator).
	UINT32	uThreadNameLen;		
};
typedef struct SNPS3_PPU_THREAD_INFO  SNPS3_PPU_THREAD_INFO;
typedef SNPS3_PPU_THREAD_INFO *PSNPS3_PPU_THREAD_INFO;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3_SPU_THREAD_INFO
///	@brief	This structure contains the parameters that define an SPU thread. It is
///			used in the function SNPS3ThreadInfo() function to obtain information about a
///			particular thread.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_SPU_THREAD_INFO
{
	/// SPU thread group that this thread belongs to.
	UINT32	uThreadGroupID;		
	/// Thread ID.
	UINT32	uThreadID;			
	/// Length of file name (including NULL terminator).
	UINT32	uFileNameLen;		
	/// Length of thread name (including NULL terminator).
	UINT32	uThreadNameLen;		
};
typedef struct SNPS3_SPU_THREAD_INFO SNPS3_SPU_THREAD_INFO;
typedef SNPS3_SPU_THREAD_INFO *PSNPS3_SPU_THREAD_INFO;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Requests thread information.
///             For PPU units the buffer will contain an
///             \link SNPS3_PPU_THREAD_INFO SNPS3_PPU_THREAD_INFO\endlink structure followed by the thread name. For SPUs the
///             buffer will contain an \link SNPS3_SPU_THREAD_INFO SNPS3_SPU_THREAD_INFO\endlink structure followed by the file
///             name and thread name.\n
///             Call this function with pBuffer = NULL to get the required buffer size.
///             To get the thread information, set pBuffer to point to an allocated memory block of size *puBufferSize,
///             and call the function again.\n
///             Extended PPU thread information can be obtained using SNPS3PPUThreadInfoEx().
///
///  @param     hTarget			Handle to a valid target.
///	 @param		uUnit			Processor unit (see enum \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///	 @param		uProcessID		Process ID.
///	 @param		uThreadID		64-bit Thread ID.
///  @param		puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pBuffer			Pointer to a buffer to receive the information. 
///                             As a guide, allocate buffer size of at least 1k.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved thread information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_UNIT - uUnit not valid ID (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer or uProcessID invalid value.\n
///             SN_E_OUT_OF_MEM - Size of buffer given by puBufferSize was smaller than required size.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadInfo(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID,
							   UINT32 *puBufferSize, BYTE *pBuffer);

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3_PPU_THREAD_INFO_EX
/// @brief	This structure contains extended information for a PPU thread. It is used in the
///			function SNPS3PPUThreadInfoEx() to obtain information about a particular thread.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_PPU_THREAD_INFO_EX
{
	/// Thread ID.
	UINT64	uThreadID;			
	/// Current priority, normally the same as base priority, only differs when thread's priority is elevated due to priority inheritance support.
	UINT32	uPriority;	
	/// Base priority.
	UINT32 uBasePriority;
	/// Current state (see defines SNPS3_PPU_IDLE to SNPS3_PPU_DELETED).
	UINT32	uState;				
	/// Start address of stack.
	UINT64	uStackAddress;		
	/// Stack size (in bytes).
	UINT64	uStackSize;			
	/// Length of thread name (including NULL terminator).
	UINT32	uThreadNameLen;		
};
typedef struct SNPS3_PPU_THREAD_INFO_EX  SNPS3_PPU_THREAD_INFO_EX;
typedef SNPS3_PPU_THREAD_INFO_EX *PSNPS3_PPU_THREAD_INFO_EX;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Requests extended information about a PPU thread.
///             The buffer will contain an \link SNPS3_PPU_THREAD_INFO_EX SNPS3_PPU_THREAD_INFO_EX\endlink structure followed by the thread name.\n
///             Call this function with pBuffer = NULL to get the required buffer size.
///             To get the thread information, set pBuffer to point to an allocated memory block of size *puBufferSize,
///             and call the function again.\n
///             To get standard thread information use SNPS3ThreadInfo() with unit set to PS3_UI_CPU.
///
///  @param     hTarget			Handle to a valid target.
///	 @param		uProcessID		Process ID.
///	 @param		uThreadID		64-bit Thread ID.
///  @param		puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pBuffer			Pointer to a buffer to receive the information. 
///                             As a guide, allocate buffer size of at least 1k.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved PPU thread information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer or uProcessID invalid value.\n
///             SN_E_OUT_OF_MEM - Size of buffer given by puBufferSize was smaller than required.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3PPUThreadInfoEx(HTARGET hTarget, UINT32 uProcessID, UINT64 uThreadID, UINT32 *puBufferSize, BYTE *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Restarts specified thread.
///             Note: SPU mode only included for backwards compatibility and only works
///             if thread group is stopped, instead use SNPS3ClearSPULoopPoint() with bCurrentPC set to true.
///
///  @param     hTarget     Handle to a valid target.
///  @param     uUnit		Processor unit (see enum \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param		uProcessID	Process ID.
///  @param		uThreadID	64-bit Thread ID.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully continued thread.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - uProcessID was invalid value.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadContinue(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Stops specified thread.
///             Note: SPU mode only included for backwards compatibility and only works
///             if thread group is stopped, instead use SNPS3SetSPULoopPoint() with bCurrentPC set to true.
///
///  @param     hTarget     Handle to a valid target.
///  @param     uUnit		Processor unit (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param		uProcessID	Process ID.
///  @param		uThreadID	64-bit Thread ID.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully stopped thread.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - uProcessID was invalid value.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadStop(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Reads an array of registers from the specified thread.
///             Thread must be stopped before calling this function. See SNPS3ThreadStop().
///
///  @param     hTarget     Handle to a valid target.
///  @param     uUnit		Processor unit (see enum \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param		uProcessID	Process ID.
///  @param		uThreadID	64-bit Thread ID.
///  @param		uNumRegisters
///                         Number of registers to get.
///  @param		puNum		Pointer to buffer holding array of register numbers to get.
///  @param		pRegBuffer	Buffer to hold values returned.
///             			Note: this buffer must be big enough to hold the 
///             			returned registers. Allocate a buffer of
///             			uNumRegisters * SNPS3_REGLEN.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved registers.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puNum or pReg buffer were NULL pointers, process ID/thread ID not a valid value or
///             invalid register number supplied.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_BAD_UNIT - Unit value not valid (see enum \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadGetRegisters(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID, 
									   UINT32 uNumRegisters, UINT32 *puNum, BYTE *pRegBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets the registers for a thread.
///             Thread must be stopped before calling this function. See SNPS3ThreadStop().
///
///  @param     hTarget     Handle to a valid target.
///  @param     uUnit		Processor unit (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param		uProcessID	Process ID.
///  @param		uThreadID	64-bit Thread ID.
///  @param		uNumRegisters
///                         Number of registers to set.
///  @param		puNum		Pointer to buffer holding array of register
///                         numbers to set.
///  @param		pRegBuffer	Buffer containing values of registers to set.
///             			Each register is of size SNPS3_REGLEN.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set registers.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puNum or pReg buffer were NULL pointers or process ID/thread ID -1 value.\n
///             SN_E_COMMS_ERR - Process ID/thread ID invalid value or an unknown problem occurred communicating with the target.\n 
///             SN_E_BAD_UNIT - Unit value not valid (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadSetRegisters(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID, 
									   UINT32 uNumRegisters, UINT32 *puNum, const BYTE *pRegBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Enables and disables automatic client notification of unit 
///             status changes.
///             Normally, Target Manager will notify all registered clients 
///             whenever another client modifies memory or register contents. 
///             It does this by sending a SN_TGT_EVENT_UNIT_STATUS_CHANGE notification for 
///             that unit.\n
///             In some circumstances this may be undesirable, for example if the 
///             TMAPI application is carrying out many memory writes, and the 
///             ProDG Debugger is connected at the same time, it will 
///             automatically refresh itself for each update and slow down 
///             the SDK function. For these cases, notifications can be 
///             disabled by calling this function.\n
///             This feature is enabled by default. Once set it remains set 
///             for the lifetime of the Target Manager and is reset to 
///             enabled when restarted. The previous 
///             setting should be restored at the end of the SDK application.
///
///  @param     hTarget        Handle to target.
///  @param     uEnabled       New state for auto notifications. 0 = disabled, 1 = enabled.
///  @param     puPrevState    Pointer to a buffer to receive previous state.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully enabled / disabled status update.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3EnableAutoStatusUpdate(HTARGET hTarget, UINT32 uEnabled, UINT32 *puPrevState);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets the default primary PPU thread stack size for the target.
///
///  @param     hTarget        Handle to target.
///  @param     uSize          Size of the stack (should be one of the 
///                            predefined constants, see SNPS3_ELF_STACK_SIZE_*).
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set stack size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Invalid size.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetDefaultPPUThreadStackSize(HTARGET hTarget, UINT32 uSize);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets the default primary PPU thread stack size for the target.
///
///  @param     hTarget        Handle to target.
///  @param     puSize         Pointer to a UINT32 to hold the size.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved stack size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puSize was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDefaultPPUThreadStackSize(HTARGET hTarget, UINT32 *puSize);

// @name <TargetPowerState>
//@{
//! Target power state.
#define SNPS3_POWER_STATE_OFF				0
#define SNPS3_POWER_STATE_ON				1
#define SNPS3_POWER_STATE_SUSPENDED			2
#define SNPS3_POWER_STATE_UNKNOWN			4
#define SNPS3_POWER_STATE_SWITCHING_ON		5
//@}

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Retrieves the current power status of the specified target.
///             See SNPS3_POWER_STATE_OFF etc.
///
///  @param     hTarget        Handle to target.
///  @param     pnStatus       Pointer to a long to hold the status.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved power status.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pnStatus is null pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///             	
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetPowerStatus(HTARGET hTarget, long *pnStatus);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Powers on the specified target.
///
///  @param     hTarget        Handle to target.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version (must have WOL enabled).\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully powered on.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_S_NO_ACTION - Already powered on.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3PowerOn(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Powers off the specified target.
///
///  @param     hTarget        Handle to target.
///  @param     uForce         If non-zero, target will be forced off.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version (must have Wake-On-LAN (WOL) enabled).\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully powered off target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_S_NO_ACTION - Already powered off (reference tool).\n
///             SN_E_NOT_CONNECTED - Already powered off (debugging station).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3PowerOff(HTARGET hTarget, UINT32 uForce);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Resets specified target allowing specification of boot, reset and system parameters.
///
///  @param     hTarget     Handle to a valid target.
///  @param     uBoot	    See SNPS3TM_BOOTP_DEFAULT to SNPS3TM_BOOTP_NIC_DUAL.
///  @param     uBootMask   See SNPS3TM_BOOTP_BOOT_MODE_MASK to SNPS3TM_BOOTP_ALL_MASK
///  @param     uReset      PS3 reset parameters (SNPS3TM_RESETP_*).
///  @param     uResetMask  Mask for reset parameters, or SNPS3TM_RESETP_ALL_MASK.
///  @param     uSystem     PS3 system parameters (SNPS3TM_SYSTEMP_*)
///  @param     uSystemMask Mask for system parameters (typically SNPS3TM_SYSTEMP_MODEL_MASK).
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully reset target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ResetEx(HTARGET hTarget, UINT64 uBoot, UINT64 uBootMask, UINT64 uReset, UINT64 uResetMask, 
							UINT64 uSystem, UINT64 uSystemMask);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Query the current reset parameter settings.
///             See SNPS3_BOOT_MODE_DBG, SNPS3_BOOT_MODE_MASK etc.\n
///             Boot and System parameters are the parameters which will be used
///             on the next reset (having already been masked). The boot and system
///             mask parameters are the masks which were used on the last call to 
///             SNPS3ResetEx(). The reset parameter and masks are those used in the last
///             reset.
///
///  @param     hTarget      Handle to a valid target.
///  @param     puBoot	     See SNPS3TM_BOOTP_DEFAULT to SNPS3TM_BOOTP_NIC_DUAL.
///  @param     puBootMask   See SNPS3TM_BOOTP_BOOT_MODE_MASK to SNPS3TM_BOOTP_ALL_MASK
///  @param     puReset      PS3 reset parameters (SNPS3TM_RESETP_*).
///  @param     puResetMask  Mask for reset parameters, or SNPS3TM_RESETP_ALL_MASK.
///  @param     puSystem     PS3 system parameters (SNPS3TM_SYSTEMP_*).
///  @param     puSystemMask Mask for system parameters (typically SNPS3TM_SYSTEMP_MODEL_MASK).
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved reset parameters.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - One of the pointers was null.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetResetParameters(HTARGET hTarget, UINT64 *puBoot, UINT64 *puBootMask, 
									   UINT64 *puReset, UINT64 *puResetMask, UINT64 *puSystem, UINT64 *puSystemMask);


// @name <BootParameter>
//@{
//! Default boot parameter.
const UINT64 SNPS3TM_BOOTP_DEFAULT				= 0x0000000000000000;

//! System mode.
const UINT64 SNPS3TM_BOOTP_SYSTEM_MODE			= 0x0000000000000011;

//! Release mode.
const UINT64 SNPS3TM_BOOTP_RELEASE_MODE			= 0x0000000000000001;

//! Debug mode.
const UINT64 SNPS3TM_BOOTP_DEBUG_MODE			= 0x0000000000000010;

//! Limit available memory to console setting (default = tool setting).
const UINT64 SNPS3TM_BOOTP_MEM_CONSOLE			= 0x0000000000000002;

//! Use BD drive (default = use BD emulator).
const UINT64 SNPS3TM_BOOTP_BD_EMU_OFF			= 0x0000000000000004;

//! Match transfer rate of BD emulator (default = HDD native).
const UINT64 SNPS3TM_BOOTP_TRANSFER_RATE_BD		= 0x0000000000000008;

//! Use USB mass storage BD emulator (default = Internal HDD (DEV)).
const UINT64 SNPS3TM_BOOTP_BD_EMU_USB			= 0x0000000000000020;

//! Use game port (default = DEV LAN).
const UINT64 SNPS3TM_BOOTP_HOSTFS_TARGET		= 0x0000000000000040;

//! Default = single.
const UINT64 SNPS3TM_BOOTP_NIC_DUAL				= 0x0000000000000080;
//@}

// @name <BootParameterMasks>
//@{
//! See \link SNPS3TM_BOOTP_SYSTEM_MODE SNPS3TM_BOOTP_SYSTEM_MODE\endlink, \link SNPS3TM_BOOTP_RELEASE_MODE SNPS3TM_BOOTP_RELEASE_MODE\endlink and \link SNPS3TM_BOOTP_DEBUG_MODE SNPS3TM_BOOTP_DEBUG_MODE\endlink.
const UINT64 SNPS3TM_BOOTP_BOOT_MODE_MASK		= 0x0000000000000011;

//! See \link SNPS3TM_BOOTP_MEM_CONSOLE SNPS3TM_BOOTP_MEM_CONSOLE\endlink.
const UINT64 SNPS3TM_BOOTP_MEMSIZE_MASK			= 0x0000000000000002;

//! See \link SNPS3TM_BOOTP_BD_EMU_OFF SNPS3TM_BOOTP_BD_EMU_OFF\endlink.
const UINT64 SNPS3TM_BOOTP_BLURAY_EMU_MASK		= 0x0000000000000004;

//! See \link SNPS3TM_BOOTP_TRANSFER_RATE_BD SNPS3TM_BOOTP_TRANSFER_RATE_BD\endlink.
const UINT64 SNPS3TM_BOOTP_HDD_SPEED_MASK		= 0x0000000000000008;

//! See \link SNPS3TM_BOOTP_BD_EMU_USB SNPS3TM_BOOTP_BD_EMU_USB\endlink.
const UINT64 SNPS3TM_BOOTP_BLURAY_EMU_SEL_MASK	= 0x0000000000000020;

//! See \link SNPS3TM_BOOTP_HOSTFS_TARGET SNPS3TM_BOOTP_HOSTFS_TARGET\endlink.
const UINT64 SNPS3TM_BOOTP_HOSTFS_MASK			= 0x0000000000000040;

//! See \link SNPS3TM_BOOTP_NIC_DUAL SNPS3TM_BOOTP_NIC_DUAL\endlink.
const UINT64 SNPS3TM_BOOTP_NIC_MASK				= 0x0000000000000080;

//! Mask for all boot parameters.
const UINT64 SNPS3TM_BOOTP_ALL_MASK				=	SNPS3TM_BOOTP_BOOT_MODE_MASK | SNPS3TM_BOOTP_MEMSIZE_MASK | SNPS3TM_BOOTP_BLURAY_EMU_MASK 
													| SNPS3TM_BOOTP_HDD_SPEED_MASK | SNPS3TM_BOOTP_BLURAY_EMU_SEL_MASK | SNPS3TM_BOOTP_HOSTFS_MASK 
													| SNPS3TM_BOOTP_NIC_MASK;
//@}

//! @deprecated - use \link SNPS3TM_BOOTP_BOOT_MODE_MASK SNPS3TM_BOOTP_BOOT_MODE_MASK\endlink.
#define SNPS3_BOOT_MODE_MASK			SNPS3_BOOT_MODE_MASK_deprecated_instead_use_SNPS3TM_BOOTP_BOOT_MODE_MASK
//! @deprecated - use \link SNPS3TM_BOOTP_MEMSIZE_MASK SNPS3TM_BOOTP_MEMSIZE_MASK\endlink.
#define SNPS3_BOOT_MEMSIZE_MASK			SNPS3_BOOT_MEMSIZE_MASK_deprecated_instead_use_SNPS3TM_BOOTP_MEM_CONSOLE
//! @deprecated - use \link SNPS3TM_BOOTP_BLURAY_EMU_MASK SNPS3TM_BOOTP_BLURAY_EMU_MASK\endlink.
#define SNPS3_BOOT_BD_EMU_MASK			SNPS3_BOOT_BD_EMU_MASK_deprecated_instead_use_SNPS3TM_BOOTP_BD_EMU_OFF
//! @deprecated - use \link SNPS3TM_BOOTP_HDD_SPEED_MASK SNPS3TM_BOOTP_HDD_SPEED_MASK\endlink.
#define SNPS3_BOOT_BD_HDDSPEED_MASK		SNPS3_BOOT_BD_HDDSPEED_MASK_deprecated_instead_use_SNPS3TM_BOOTP_TRANSFER_RATE_BD
//! @deprecated - use \link SNPS3TM_BOOTP_BLURAY_EMU_SEL_MASK SNPS3TM_BOOTP_BLURAY_EMU_SEL_MASK\endlink.
#define SNPS3_BOOT_BD_EMU_USB_MASK		SNPS3_BOOT_BD_EMU_USB_MASK_deprecated_instead_use_SNPS3TM_BOOTP_BD_EMU_USB
//! @deprecated - use \link SNPS3TM_BOOTP_NIC_MASK SNPS3TM_BOOTP_NIC_MASK\endlink.
#define SNPS3_BOOT_NIC_MASK				SNPS3_BOOT_NIC_MASK_deprecated_instead_use_SNPS3TM_BOOTP_NIC_MASK
//! @deprecated - use \link SNPS3TM_BOOTP_ALL_MASK SNPS3TM_BOOTP_ALL_MASK\endlink.
#define SNPS3_BOOT_DEFAULT_MASK			SNPS3_BOOT_DEFAULT_MASK_deprecated_instead_use_SNPS3TM_BOOTP_ALL_MASK

//! @deprecated - use \link SNPS3TM_BOOTP_DEFAULT SNPS3TM_BOOTP_DEFAULT\endlink.
#define SNPS3_DEF_BOOT_PARAM			SNPS3_DEF_BOOT_PARAM_deprecated_instead_use_SNPS3TM_BOOTP_DEFAULT
//! @deprecated - use \link SNPS3TM_BOOTP_DEBUG_MODE SNPS3TM_BOOTP_DEBUG_MODE\endlink.
#define SNPS3_BOOT_MODE_DBG				SNPS3_BOOT_MODE_DBG_deprecated_instead_use_SNPS3TM_BOOTP_DEBUG_MODE 
//! @deprecated - use \link SNPS3TM_BOOTP_SYSTEM_MODE SNPS3TM_BOOTP_SYSTEM_MODE\endlink.
#define SNPS3_BOOT_MODE_SYS				SNPS3_BOOT_MODE_SYS_deprecated_instead_use_SNPS3TM_BOOTP_SYSTEM_MODE 
//! @deprecated.
#define SNPS3_BOOT_MEMSIZE_TOOL			SNPS3_BOOT_MEMSIZE_TOOL_deprecated_instead_use_0x00ULL
//! @deprecated - use \link SNPS3TM_BOOTP_MEM_CONSOLE SNPS3TM_BOOTP_MEM_CONSOLE\endlink.
#define SNPS3_BOOT_MEMSIZE_CONSOLE		SNPS3_BOOT_MEMSIZE_CONSOLE_deprecated_instead_use_SNPS3TM_BOOTP_MEM_CONSOLE
//! @deprecated.
#define SNPS3_BOOT_BD_EMU				SNPS3_BOOT_BD_EMU_deprecated_instead_use_0x00ULL
//! @deprecated - use \link SNPS3TM_BOOTP_BD_EMU_OFF SNPS3TM_BOOTP_BD_EMU_OFF\endlink.
#define SNPS3_BOOT_BD_DRIVE				SNPS3_BOOT_BD_DRIVE_deprecated_instead_use_SNPS3TM_BOOTP_BD_EMU_OFF
//! @deprecated - use \link SNPS3TM_BOOTP_TRANSFER_RATE_BD SNPS3TM_BOOTP_TRANSFER_RATE_BD\endlink.
#define SNPS3_BOOT_HDDSPEED_EMU			SNPS3_BOOT_HDDSPEED_EMU_deprecated_instead_use_SNPS3TM_BOOTP_TRANSFER_RATE_BD 
//! @deprecated.
#define SNPS3_BOOT_HDDSPEED_NATIVE		SNPS3_BOOT_HDDSPEED_NATIVE_deprecated_instead_use_0x00ULL
//! @deprecated.
#define SNPS3_BOOT_BD_EMU_DEV			SNPS3_BOOT_BD_EMU_DEV_deprecated_instead_use_0x00ULL	
//! @deprecated - use \link SNPS3TM_BOOTP_BD_EMU_USB SNPS3TM_BOOTP_BD_EMU_USB\endlink.
#define SNPS3_BOOT_BD_EMU_USB			SNPS3_BOOT_BD_EMU_USB_deprecated_instead_use_SNPS3TM_BOOTP_BD_EMU_USB
//! @deprecated.
#define SNPS3_BOOT_HOSTFS_DEV			SNPS3_BOOT_HOSTFS_DEV_deprecated_instead_use_0x00ULL
//! @deprecated - use \link SNPS3TM_BOOTP_HOSTFS_TARGET SNPS3TM_BOOTP_HOSTFS_TARGET\endlink.
#define SNPS3_BOOT_HOSTFS_TARGET		SNPS3_BOOT_HOSTFS_TARGET_deprecated_instead_use_SNPS3TM_BOOTP_HOSTFS_MASK	


//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets the boot parameter currently used by the OS (use SNPS3GetResetParameters()
///             to get the parameter that will be used during the next reset).
///
///  @param     hTarget      Handle to a valid target.
///  @param     puBoot		PS3 boot parameter. See SNPS3TM_BOOTP_DEFAULT to SNPS3TM_BOOTP_NIC_DUAL.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved boot parameter.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBoot was null.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetCurrentBootParameter(HTARGET hTarget, UINT64 *puBoot);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets the target boot parameter.
///
///  @param     hTarget      Handle to a valid target.
///  @param     uBoot	     PS3 boot parameter. See SNPS3TM_BOOTP_DEFAULT to SNPS3TM_BOOTP_NIC_DUAL.
///  @param     uBootMask    PS3 boot mask. See SNPS3TM_BOOTP_BOOT_MODE_MASK to SNPS3TM_BOOTP_ALL_MASK.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set boot parameter.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetBootParameter(HTARGET hTarget, UINT64 uBoot, UINT64 uBootMask);

// @name <SystemParameters>
//@{
//! System parameter.
const UINT64 SNPS3TM_SYSTEMP_TARGET_MODEL_60GB	= 0x0001000000000000ui64;
const UINT64 SNPS3TM_SYSTEMP_TARGET_MODEL_20GB	= 0x0002000000000000ui64;
const UINT64 SNPS3TM_SYSTEMP_RELEASE_CHECK_MODE = 0x0000800000000000ui64;
//@}

// @name <SystemParameterMasks>
//@{
//! System parameter model mask.
const UINT64 SNPS3TM_SYSTEMP_MODEL_MASK			= 0x00ff000000000000ui64;
//! System parameter model mask.
const UINT64 SNPS3TM_SYSTEMP_RELEASE_CHECK_MASK	= 0x0000800000000000ui64;
//! System parameter mask.
const UINT64 SNPS3TM_SYSTEMP_ALL_MASK			= SNPS3TM_SYSTEMP_MODEL_MASK | SNPS3TM_SYSTEMP_RELEASE_CHECK_MASK;
//@}

SNDEPRECATED("use SNPS3TM_SYSTEMP_MODEL_MASK instead")
//! @deprecated - use \link SNPS3TM_SYSTEMP_MODEL_MASK SNPS3TM_SYSTEMP_MODEL_MASK\endlink.
const UINT64 SNPS3TM_SYSTEMP_TARGET_MODEL		= 0x00ff000000000000ui64;

//! @deprecated - use \link SNPS3TM_SYSTEMP_MODEL_MASK SNPS3TM_SYSTEMP_MODEL_MASK\endlink.
#define	 SNPS3_SYSTEM_TARGET_MODEL_MASK			SNPS3_SYSTEM_TARGET_MODEL_MASK_deprecated_instead_use_SNPS3TM_SYSTEMP_MODEL_MASK
//! @deprecated - use \link SNPS3TM_SYSTEMP_TARGET_MODEL_60GB SNPS3TM_SYSTEMP_TARGET_MODEL_60GB\endlink.
#define  SNPS3_SYSTEM_TARGET_MODEL_60GB			SNPS3_SYSTEM_TARGET_MODEL_60GB_deprecated_instead_use_SNPS3TM_SYSTEMP_TARGET_MODEL_60GB
//! @deprecated - use \link SNPS3TM_SYSTEMP_TARGET_MODEL_20GB SNPS3TM_SYSTEMP_TARGET_MODEL_20GB\endlink.
#define  SNPS3_SYSTEM_TARGET_MODEL_20GB			SNPS3_SYSTEM_TARGET_MODEL_20GB_deprecated_instead_use_SNPS3TM_SYSTEMP_TARGET_MODEL_20GB

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets the system parameter of the given target.
///
///  @param     hTarget			   Handle to a valid target.
///  @param     uSystem			   PS3 system parameter. See SNPS3TM_SYSTEMP_*.
///  @param     uSystemMask        PS3 system mask. Typically SNPS3TM_SYSTEMP_MODEL_MASK. 
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set system parameter.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetSystemParameter(HTARGET hTarget, UINT64 uSystem, UINT64 uSystemMask);
	
//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Calls the supplied callback function for each target in the system, passing an 
///             HTARGET handle. The callback function can then call SNPS3GetTargetInfo() 
///             to retrieve relevant information for that target and store it.
///
///  @param     pfnCallBack    Pointer to callback function.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully called the callbacks.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_NO_TARGETS - There are no targets to enumerate.\n
///             SN_E_BAD_PARAM - pfnCallBack was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3EnumerateTargets(TMAPI_EnumTargetsCallback pfnCallBack);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Calls the supplied callback function for each target in the system, passing an 
///             HTARGET handle. The callback function can then call SNPS3GetTargetInfo() 
///             to retrieve relevant information for that target and store it. The
///             callback function should return 0 to continue the enumeration or non-zero
///             to cancel it.
///
///  @param     pfnCallBack    Pointer to callback function.
///  @param     pArg           Parameter passed to callback, void pointer which can be cast to anything.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully called the callbacks.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_NO_TARGETS - There are no targets to enumerate.\n
///             SN_E_BAD_PARAM - pfnCallBack was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3EnumerateTargetsEx(TMAPI_EnumTargetsExCallback pfnCallBack, void *pArg);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Register a callback function which will be called when a 
///             target event occurs (for example processes/threads created/destroyed, 
///             exceptions, and so on).
///             See SN_TGT_EVENT_* definitions for details of possible events.
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param		hTarget	      Handle to a target.
///  @param     pfnCallBack   Pointer to callback.
///  @param		pUserData     Optional user defined data.
///
///  Callback must be in the form of:
///
/// \code
///  void Callback(
///          HTARGET hTarget,   // Handle to target.
///          UINT32 Type,       // SN_EVENT_TARGET.
///          UINT32 Param,      // Reserved - will be 0.
///          UINT32 Length,     // Length of data in event including header.
///          SNRESULT eResult,  // Success or failure of data fetch.
///          BYTE* Data,        // Pointer to event data. Data will start with SN_EVENT_TARGET_HDR.
///          void* UserData);   // Optional user data.
/// \endcode
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered for target events.\n
///             SN_S_REPLACED - Already registered for target events, previous callback function has been replaced.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Pointer to callback function was null.
///             
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterTargetEventHandler(HTARGET hTarget, TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Register a callback function which will be called when a 
///             server event occurs (e.g. target added, target deleted, default target
///			    changed).\n
///				See SN_SERVER_EVENT_* definitions in APIBase.h for details of possible events. 
///				The event type is signified by the uEvent member of the \link SN_EVENT_SERVER_HDR SN_EVENT_SERVER_HDR\endlink
///				passed to the callback as the Data parameter.
///				The callback will only be invoked when SNPS3Kick() is called.
///
///  @param     pfnCallBack   Pointer to callback.
///  @param		pUserData     Optional user defined data.
///
///  Callback must be in the form of:
///
/// \code
///  void Callback(
///          HTARGET hTarget,   // Target ID, if applicable. This will have the same value as the uTarget member of the
///			 \link SN_EVENT_SERVER_HDR SN_EVENT_SERVER_HDR\endlink passed as the Data parameter.
///          UINT32 Type,       // SN_EVENT_SERVER.
///          UINT32 Param,      // Not used.
///          UINT32 Length,     // Length of data in event including header.
///          SNRESULT eResult,  // Success or failure of data fetch.
///          BYTE* Data,        // Pointer to event data. Data will start with \link SN_EVENT_SERVER_HDR SN_EVENT_SERVER_HDR\endlink.
///          void* UserData);   // Optional user data.
/// \endcode
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///				SN_S_OK - Successfully registered for server events.\n
///				SN_S_REPLACED - Already registered for server events, previous callback function has been replaced.\n
///				SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///				SN_E_BAD_PARAM - Pointer to callback function was null.
///				
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterServerEventHandler(TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Unregister the current callback function from receiving events.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///				SN_S_OK - Successfully cancelled server events.\n
///				SN_S_NO_ACTION - No callback function registered.\n
///				SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UnRegisterServerEventHandler();

#define TXID_FORCE_FLAG		0xffffffff
//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Upload the given file to the target.
///             Note:	This only initiates the upload. To check progress, success or failure,
///             		monitor FTP events (see SNPS3RegisterFTPEventHandler() for more information), or
///						call SNPS3WaitForFileTransfer() to wait on result.
///
///  @param     hTarget        Handle to target.
///  @param     pSource        Local file. UTF-8 null-terminated string.
///  @param     pDestination   Target file. UTF-8 null-terminated string.
///  @param		pTXID		   Address of UINT32 to store transaction ID.
///             			   Set the TXID value to TXID_FORCE_FLAG (0xffffffff) on input to force the transfer
///                            (ie. bypass the timestamp/size check)
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated file upload.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pSource or pDest was NULL pointer, or contained a bad path.\n
///             SN_E_DATA_TOO_LONG - pSource or pDest exceeded maximum path length for system (1024 for PS3, 260 for windows PC).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UploadFile(HTARGET hTarget, const char* pSource, const char* pDestination, UINT32* pTXID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Cancel a current or pending transfer to/from the target.
///             Note: This only initiates the cancellation. To check success or failure,
///             monitor FTP events (see SNPS3RegisterFTPEventHandler() for more information).
///				Alternatively, use SNPS3WaitForFileTransfer() to check the result.
///
///  @param     hTarget        Handle to target.
///  @param		uTXID		   Transaction ID of transfer to cancel.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated cancel of file transfer.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3CancelFileTransfer(HTARGET hTarget, UINT32 uTXID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Retry the file transfer with the given ID.
///             Note: This only initiates the transfer. To check success or failure,
///             monitor FTP events (see SNPS3RegisterFTPEventHandler() for more information).
///				Alternatively, use SNPS3WaitForFileTransfer() to check the result.
///
///  @param     hTarget        Handle to target.
///  @param		uTXID		   Transaction ID of transfer to continue.
///  @param     uForce		   Non-zero to bypass check for modifications.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated file transfer retry.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RetryFileTransfer(HTARGET hTarget, UINT32 uTXID, UINT32 uForce);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get a directory listing.
///             Call this function passing NULL for pDirectoryList to get the number of directory entries. Allocate 
///             a block of SNPS3DirEntry() structures and call this function again, 
///             passing in a pointer to the allocated buffer.
///
///  @param     pDirectory		Directory path. UTF-8 null-terminated string.
///  @param     puCount			[in] Size of buffer.\n
///                             [out] Number of directory entries for which space is required, 
///                             or if buffer is big enough, number written to buffer.
///  @param     pDirectoryList Pointer to a buffer to hold the directory list.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @result	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved directory list or number of entries.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pDirectory and/or puCount were NULL pointers.\n
///             SN_E_OUT_OF_MEM - Size of pDirectoryList array given by puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDirectoryList(HTARGET hTarget, const char* pDirectory, UINT32* puCount, SNPS3DirEntry* pDirectoryList);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get a directory listing.
///             Call this function passing NULL for pDirectoryList to get the number of directory entries. Allocate 
///             a block of SNPS3DirEntryEx() structures and call this function again, 
///             passing in a pointer to the allocated buffer.
///
///  @param     pDirectory		Directory path. UTF-8 null-terminated string.
///  @param     puCount			[in] Size of buffer.\n
///                             [out] Number of directory entries for which space is required, 
///                             or if buffer is big enough, number written to buffer.
///  @param     pDirectoryList  Pointer to a buffer to hold the directory list.
///  @param		pTZ				Pointer to structure to hold target timezone/dst settings (can be null).
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///  @result	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved directory list or number of entries.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pDirectory and/or puCount were NULL pointers.\n
///             SN_E_OUT_OF_MEM - Size of pDirectoryList array given by puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDirectoryListEx(HTARGET hTarget, const char* pDirectory, UINT32* puCount, SNPS3DirEntryEx* pDirectoryList, SNPS3TargetTimezone* pTZ);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Create a directory on the target.
///
///  @param     hTarget        Handle to target.
///  @param     pDirectory     Target directory. UTF-8 null-terminated string.
///  @param     uMode          Access mode, used as the second parameter to CellFsMkDir (see SDK).
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully created directory.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pDirectory was NULL pointer.\n
///             SN_E_DATA_TOO_LONG - pDirectory was longer than max path length (1056).\n
///             SN_E_FILE_ERROR - Error creating directory.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3MakeDirectory(HTARGET hTarget, const char* pDirectory, UINT32 uMode);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Rename a file on the target.
///
///  @param     hTarget        Handle to target.
///  @param     pSource        Target file path. UTF-8 null-terminated string.
///  @param     pDestination   New name. UTF-8 null-terminated string.
///	
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully renamed file.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pSource and/or pDestination was NULL pointer.\n
///             SN_E_DATA_TOO_LONG - pSource and/or pDestination was longer than max path length (1056).\n
///             SN_E_FILE_ERROR - Error renaming file.\n
///             SN_E_COMMS_ERR - Error communicating with target.\n
///             SN_E_NOT_CONNECTED - Not connected to target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Rename(HTARGET hTarget, const char* pSource, const char* pDestination);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3Protocol
///  @brief     Structure for protocol information.  This should be treated as
///             an opaque structure.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3Protocol
{
	/// Protocol number.
	UINT32 uProtocol;
	/// Port number.
	UINT32 uPort;
	/// LPAR Description.
	UINT32 uLPARDesc;
};
typedef struct SNPS3Protocol SNPS3Protocol;

//! Default protocol priority for registering custom protocols.
const UINT32 SNPS3_DEFAULT_PROTO_PRIORITY = 0x80;

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated	Use SNPS3CustomProtocolCallbackEx (with SNPS3RegisterCustomProtocolEx).
///  @brief			Use SNPS3CustomProtocolCallbackEx (with SNPS3RegisterCustomProtocolEx).
///
//////////////////////////////////////////////////////////////////////////////

typedef void (__stdcall *SNPS3CustomProtocolCallback)(HTARGET hTarget, BYTE* pData, UINT32 uLength, void* pUser);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Callback function for registered protocols.
///             There can be at most one function for each registered protocol. 
///             pData points to a temporary custom protocol data buffer of length uLength.
///             The callback will only be invoked when SNPS3Kick is called.
///
//////////////////////////////////////////////////////////////////////////////

typedef void (__stdcall *SNPS3CustomProtocolCallbackEx)(HTARGET hTarget, SNPS3Protocol protocol, BYTE* pData, UINT32 uLength, void* pUser);

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated	Use SNPS3RegisterCustomProtocolEx.
///  @brief			Use SNPS3RegisterCustomProtocolEx().
///
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3RegisterCustomProtocolEx instead")
SNAPI SNRESULT SNPS3RegisterCustomProtocol(HTARGET hTarget, UINT32 uProtocol, UINT32 uPort, const char* pszLPAR, 
										   UINT32 uPriority, SNPS3Protocol *pProto, 
										   SNPS3CustomProtocolCallback pfnCallBack, void *pUser);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Register a custom protocol on a target, and a callback for custom protocol notifications.
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param     hTarget        Handle to target.
///  @param     uProtocol      Protocol identifier.
///  @param     uPort          Protocol port.
///  @param     pszLPAR        LPAR descriptor.
///  @param     uPriority      Protocol priority. Typically SNPS3_DEFAULT_PROTO_PRIORITY.
///  @param     pProto         Structure into which to write the details of
///                            the protocol.
///  @param     pfnCallBack    Callback function to invoke when data is
///                            received on the given protocol (one callback per protocol per target).
///  @param     pUser          User data to be passed into the callback function.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered for custom protocol notifications.\n
///             SN_S_REPLACED - Protocol already registered on the target, previous callback function has been replaced.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszLPAR, pProto or pfnCallBack was NULL pointer.\n
///             SN_E_PROTOCOL_ALREADY_REGISTERED - Tried to register a second protocol on a debug station.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterCustomProtocolEx(HTARGET hTarget, UINT32 uProtocol, UINT32 uPort, const char* pszLPAR, 
										   UINT32 uPriority, SNPS3Protocol *pProto, 
										   SNPS3CustomProtocolCallbackEx pfnCallBack, void *pUser);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Send data to the target using the specified protocol.
///
///  @param     hTarget        Handle to target.
///  @param     pProto         Protocol information.
///  @param     pData          Data to send.
///  @param     uLength        Length of the data to send.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully sent custom protocol data.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pProto and/or pData was NULL pointer.\n
///             SN_E_DATA_TOO_LONG - Data longer than max size (reference tool: 65519 bytes, debug station: 4080 bytes).\n
///             SN_E_NOT_LISTED - Protocol not registered.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SendCustomProtocolData(HTARGET hTarget, 
										   SNPS3Protocol* pProto, BYTE* pData, UINT32 uLength);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Unregister a custom protocol callback.
///             If there are other clients registered for the same protocol,
///             the protocol will remain on the target and other clients will continue to receive notifications.
///             Otherwise, the protocol is cleared from the target.
///             Note that attempting to unregister an invalid protocol does not return an error code - it returns SN_S_NO_ACTION.
///
///  @param     hTarget        Handle to target.
///  @param     pProto         Protocol information.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered protocol and cleared it from the target.\n
///             SN_S_TARGET_STILL_REGISTERED - Protocol was unregistered but not cleared from the target, 
///             because other clients are registered for it.\n
///             SN_S_NO_ACTION - Protocol not currently registered.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pProto was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UnRegisterCustomProtocol(HTARGET hTarget, SNPS3Protocol* pProto);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Unregister the protocol, clearing it from the target.
///             If there are multiple clients registered for the protocol, they will
///             be notified internally of their unsubscription.
///
///  @param     hTarget        Handle to target.
///  @param     pProto         Protocol information.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered protocol and cleared it from the target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_NOT_LISTED - Protocol not currently registered.\n
///             SN_E_BAD_PARAM - pProto was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ForceUnRegisterCustomProtocol(HTARGET hTarget, SNPS3Protocol* pProto);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get a count of all the synchronisation primitives owned by the given process.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     pNumMutexes		Address of UINT32 to store the number of mutexes
///                             (can be NULL).
///  @param     pNumCondVars	Address of UINT32 to store the number of conditional
///                             variables (can be NULL).
///  @param     pNumRWLocks		Address of UINT32 to store the number of read-write
///                             locks (can be NULL).
///  @param     pNumLWMutexes	Address of UINT32 to store the number of light-weight
///                             mutexes (can be NULL).
///  @param     pNumEventQueues	Address of UINT32 to store the number of event 
///                             queues (can be NULL).
///  @param     pNumSemaphores	Address of UINT32 to store the number of semaphores
///                             (can be NULL).
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.8.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved synchronisation primitive counts.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSyncPrimitiveCounts(HTARGET hTarget, UINT32 uProcessID,
										   UINT32* pNumMutexes, UINT32* pNumCondVars, UINT32* pNumRWLocks,
										   UINT32* pNumLWMutexes, UINT32* pNumEventQueues, UINT32* pNumSemaphores);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list of mutexes associated with the process.
///             Call this function passing NULL for pBuffer to get the number of mutexes. Allocate 
///             a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Number of mutexes buffer can hold.\n
///             			[out] Number of mutexes for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of mutex IDs.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved mutex list or number of mutexes.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMutexList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);
 
//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3MutexAttr
///  @brief     Mutex attribute data.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MutexAttr {
	UINT32	uAttrProtocol;
	UINT32	uAttrRecursive;
	UINT32	uAttrPshared;
	UINT32	uAttrAdaptive;
	UINT64	uKey;
	UINT32	uFlags;
	char	aName[8];
};
typedef struct SNPS3MutexAttr SNPS3MutexAttr;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3MutexInfo
///  @brief     Contains parameters which define a mutex. 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MutexInfo {
	UINT32		uMutexID;
	SNPS3MutexAttr Attribute;
	UINT64		uOwnerThID;
	UINT32		uLockCounter;
	UINT32		uCondRefCounter;
	UINT32		uCondID;
	UINT32		uWaitThreadsNum;
	UINT32		uWaitAllThreadsNum;
	UINT64		aWaitThreadIDs[];
};
typedef struct SNPS3MutexInfo SNPS3MutexInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given mutex.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the mutex information, set pInfo to point to an allocated memory block of size *puBufferSize,
///             and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     uMutexID		Mutex ID.
///  @param     puBufferSize	[in] Size of buffer.\n 
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo			Pointer to a buffer holding the information
///                             about the mutex.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved mutex information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMutexInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uMutexID, UINT32 *puBufferSize, SNPS3MutexInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3CondAttr
///  @brief     Attributes for conditional variable.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3CondAttr
{
	UINT32	uAttrPshared;
	UINT64	uKey;
	UINT32	uFlags;
	char	aName[8];
};
typedef struct SNPS3CondAttr SNPS3CondAttr;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3ConditionalInfo
///  @brief     Contains parameters which define a conditional variable.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3ConditionalInfo
{
	UINT32		  uCondID;
	SNPS3CondAttr Attribute;
	UINT32		  uMutexID;
	UINT32		  uWaitThreadsNum;
	UINT32		  uWaitAllThreadsNum;
	UINT64		  aWaitThreadIDs[];
};
typedef struct SNPS3ConditionalInfo SNPS3ConditionalInfo;

//////////////////////////////////////////////////////////////////////////////
/// 
///   @brief	Get the list of conditional variables associated with the given process.
///             Call this function passing NULL for pBuffer to get the number of variables. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///  
///   @param     hTarget	Handle to target.
///   @param     uProcessID Process ID.
///   @param     puCount    [in] Number of conditional variables buffer can hold.\n
///             			[out] Number of conditional variables for which space is required, or number written to buffer.
///   @param     pBuffer    Buffer to hold the set of conditional variable IDs.
///  
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved conditional variable list or number of conditional variables.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetConditionalVariableList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given conditional variable.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the conditional variable information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     uCondVarID		Conditional variable ID.
///  @param     puBufferSize	[in] Size of buffer provided.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo			Buffer to hold the information about the conditional variable.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved conditional variable information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetConditionalVariableInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uCondVarID, 
											   UINT32 *puBufferSize, SNPS3ConditionalInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list of read-write lock list associated with the given process.
///             Call this function passing NULL for pBuffer to get the number of read-write locks. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Number of IDs buffer can hold.\n
///             			[out] Number of read-write locks for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of read-write lock IDs.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved read-write lock list or number of read-write locks.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetReadWriteLockList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3RWLockAttr
///  @brief     Read/Write lock attributes.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3RWLockAttr {
	UINT32	uAttrProtocol;
	UINT32	uAttrPshared;
	UINT64	uKey;
	UINT32	uFlags;
	char	aName[8];
};
typedef struct SNPS3RWLockAttr SNPS3RWLockAttr;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3RWLockInfo
///  @brief     Contains parameters which define a read-write lock. 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3RWLockInfo {
	UINT32		uRwlockID;
	SNPS3RWLockAttr Attribute;
	UINT64		uOwnerThID;
	UINT32		uRwaitThreadsNum;
	UINT32		uRwaitAllThreadsNum;
	UINT32		uWwaitThreadsNum;
	UINT32		uWaitAllThreadsNum;  // Should be uWwaitAllThreadsNum
	UINT64		aWaitThreadIDs[];
};
typedef struct SNPS3RWLockInfo SNPS3RWLockInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given read-write lock.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the read-write lock information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     uRWLockID		Read-Write Lock ID.
///  @param     puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo			Pointer to a buffer holding the information about the read-write lock.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved read-write lock information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetReadWriteLockInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uRWLockID, UINT32 *puBufferSize, SNPS3RWLockInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list of light-weight mutexes associated with the process.
///             Call this function passing NULL for pBuffer to get the number of light-weight mutexes. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Size of buffer.\n
///             			[out] Number of light-weight mutexes for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of light-weight mutex IDs.
///
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved light-weight mutex list or number of light-weight mutexes.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetLightWeightMutexList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3LWMutexAttr
///  @brief     Lightweight mutex attributes.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3LWMutexAttr {
	UINT32	uAttrProtocol;
	UINT32	uAttrRecursive;
	char	aName[8];
};
typedef struct SNPS3LWMutexAttr SNPS3LWMutexAttr;


//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3LWMutexInfo
///  @brief     Contains parameters which define a light weight mutex.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3LWMutexInfo {
	UINT32		uLwmutexID;
	SNPS3LWMutexAttr	Attribute;
	UINT64		uOwnerThID;
	UINT32		uLockCounter;
	UINT32		uWaitThreadsNum;
	UINT32		uWaitAllThreadsNum;
	UINT64		aWaitThreadIDs[];
};
typedef struct SNPS3LWMutexInfo SNPS3LWMutexInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given light-weight mutex.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the light-weight mutex information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget	     Handle to target.
///  @param     uProcessID   Process ID.
///  @param     uLWMutexID   Light-weight mutex ID
///  @param     puBufferSize [in] Size of the buffer.\n
///             			 [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo        Pointer to a buffer holding the information about
///                          the light-weight mutex.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved light-weight mutex information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetLightWeightMutexInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uLWMutexID, UINT32 *puBufferSize, SNPS3LWMutexInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list of event queues associated with the process.
///             Call this function passing NULL for pBuffer to get the number of event queues. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Size of buffer.\n
///             			[out] Number of event queues for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of event queue IDs.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved event queue list or number of event queues.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetEventQueueList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3EventQueueAttr
///  @brief     Event queue attributes.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EventQueueAttr
{
	UINT32	uAttrProtocol;
	UINT32	uType;
	char	aName[8];
};
typedef struct SNPS3EventQueueAttr SNPS3EventQueueAttr;


//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3SysEvent
///  @brief     System event.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3SysEvent
{
	UINT64	uSource;
	UINT64	uData1;
	UINT64	uData2;
	UINT64	uData3;
};
typedef struct SNPS3SysEvent SNPS3SysEvent;


//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3EventQueueInfo
///  @brief     Contains parameters which define an event queue.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EventQueueInfo
{
	UINT32		uEventQueueID;
	SNPS3EventQueueAttr Attribute;
	UINT64		uKey;
	UINT32		uQueueSize;
	UINT32		uWaitThreadsNum;
	UINT32		uWaitAllThreadsNum;
	UINT32		uReadableEqueueNum;
	UINT32		uReadableAllEqueueNum;
	UINT64*		aWaitThreadIDs;
	SNPS3SysEvent* aEventQueueEntries;
};
typedef struct SNPS3EventQueueInfo SNPS3EventQueueInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given event queue.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the event queue information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     uEventQueueID	Event queue ID.
///  @param     puBufferSize	[in] Size of the buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo			Pointer to a buffer holding the information about
///                             the event queue.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved event queue information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetEventQueueInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uEventQueueID, UINT32 *puBufferSize, SNPS3EventQueueInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list of semaphores associated with the process.
///             Call this function passing NULL for pBuffer to get the number of semaphores. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Size of buffer.\n
///             			[out] Number of semaphores for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of semaphore IDs.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved semaphore list or number of semaphores.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSemaphoreList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3SemaphoreAttr
///  @brief     Semaphore attributes.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3SemaphoreAttr
{
	UINT32	uAttrProtocol;
	UINT32	uAttrPshared;
	UINT64	uKey;
	UINT32	uFlags;
	char	aName[8];
};
typedef struct SNPS3SemaphoreAttr SNPS3SemaphoreAttr;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3SemaphoreInfo
///  @brief     Contains parameters which define a semaphore. 
//////////////////////////////////////////////////////////////////////////////

struct SNPS3SemaphoreInfo
{
	UINT32		uSemaphoreID;
	SNPS3SemaphoreAttr Attribute;
	UINT32		uMaxVal;
	UINT32		uCurVal;
	UINT32		uWaitThreadsNum;
	UINT32		uWaitAllThreadsNum;
	UINT64		aWaitThreadIDs[];
};
typedef struct SNPS3SemaphoreInfo SNPS3SemaphoreInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given semaphore.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the semaphore information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget	     Handle to target.
///  @param     uProcessID   Process ID.
///  @param     uSemaphoreID   Event queue ID.
///  @param     puBufferSize [in] Size of the buffer.\n
///             			 [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo        Pointer to a buffer holding the information about
///                          the semaphore.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved semaphore information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSemaphoreInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uSemaphoreID, UINT32 *puBufferSize, SNPS3SemaphoreInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list of event flags associated with the given process.
///             Call this function passing NULL for pBuffer to get the number of event flags. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Size of buffer.\n
///             			[out] Number of event flags for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of event flags.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.7.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved event flags list or number of event flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetEventFlagList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);


//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3EventFlagAttr
///  @brief     Event flag attributes.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EventFlagAttr
{
	UINT32	uAttrProtocol;
	UINT32	uAttrPshared;
	UINT64	uKey;
	UINT32	uFlags;
	UINT32	uType;
	char	aName[8];
};
typedef struct SNPS3EventFlagAttr SNPS3EventFlagAttr;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3EventFlagWaitThread
///  @brief     Contains parameters defining a thread waiting on an event flag.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EventFlagWaitThread
{
	UINT64 uThreadID;
	UINT64 uBitPattern;
	UINT32 uMode;
};
typedef struct SNPS3EventFlagWaitThread SNPS3EventFlagWaitThread;


//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3EventFlagInfo
///  @brief     Contains parameters which define an event flag.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3EventFlagInfo
{
	UINT32		uEventFlagID;
	SNPS3EventFlagAttr Attribute;
	UINT64		uBitPattern;
	UINT32		uWaitThreadsNum;
	UINT32		uWaitAllThreadsNum;
	SNPS3EventFlagWaitThread aWaitThreads[];
};
typedef struct SNPS3EventFlagInfo SNPS3EventFlagInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given event flag.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the event flag information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     uEventFlagID	Event flag ID.
///  @param     puBufferSize	[in] Size of the buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo			Pointer to a buffer holding the information about
///                             the event flag.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.7.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved event flag information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetEventFlagInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uEventFlagID, UINT32 *puBufferSize, SNPS3EventFlagInfo *pInfo);

#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Register a callback function which will be called when a 
///             file transfer event occurs.
///             See \link ApiBase::SN_EVENT_FTP SN_EVENT_FTP\endlink  and 
///             \link ApiBase::TMAPI_FT_NOTIFICATION TMAPI_FT_NOTIFICATION\endlink.
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param		hTarget	      Handle to a target.
///  @param     pfnCallBack   Pointer to callback.
///  @param		pUserData     Optional user defined data.
///
///  Callback must be in the form of:
///
///  \code
///  void Callback(
///          HTARGET hTarget,   // Handle to target.
///          UINT32 Type,       // SN_EVENT_FTP.
///          UINT32 Param,      // Reserved - will be 0.
///          SNRESULT eResult,  // Result code.
///          UINT32 Length,     // Length of data in event including header.
///          BYTE* Data,        // Pointer to event data (TMAPI_FT_NOTIFICATION*).
///          void* UserData);   // Optional user data.
///	 \endcode
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered event handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pfnCallBack was NULL pointer.\n
///             SN_S_REPLACED - Already registered for FTP events, previous callback function has been replaced.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterFTPEventHandler(HTARGET hTarget, TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Delete file or directory from target
///
///  @param     hTarget	   Handle to target.
///  @param     pPath      Path of file to delete. UTF-8 null-terminated string.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully deleted file.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pPath was NULL pointer.\n
///             SN_E_DATA_TOO_LONG - pPath exceeding maximum path length of target (1024).\n
///             SN_E_COMMS_ERR - Error communicating with target, or file could not be deleted either because it 
///             does not exist or due to permission restrictions.\n
///             SN_E_FILE_ERROR - Error deleting the file.\n
///             SN_E_TIMEOUT - Timeout occurred during the execution of the function - see SNPS3DeleteEx().\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Delete(HTARGET hTarget, const char* pPath);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Delete file or directory from target with a custom timeout. This is useful when deleting large directory
///             structures from a single base directory where the targets default timeout may not be long
///             enough for the target to remove all the files.
///
///  @param     hTarget					Handle to target.
///  @param     pPath					Path of file to delete. UTF-8 null-terminated string.
///  @param     nMillisecondTimeout     Millisecond timeout value.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully deleted file.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pPath was NULL pointer.\n
///             SN_E_DATA_TOO_LONG - pPath exceeding maximum path length of target (1024).\n
///             SN_E_COMMS_ERR - Error communicating with target, or file could not be deleted either because it 
///             does not exist or due to permission restrictions.\n
///             SN_E_FILE_ERROR - Error deleting the file.\n
///             SN_E_TIMEOUT - Timeout occurred during the execution of the function.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3DeleteEx(HTARGET hTarget, const char* pPath, const unsigned int nMillisecondTimeout);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Populates the SNPS3SystemInfo() instance supplied. 
///             Which fields have been set is indicated by the mask.
///
///  @param     hTarget	   Handle to target.
///  @param		uReserved  Reserved data.
///  @param     pInfo      Pointer to a structure to hold the information.
///  @param     puMask     Mask that indicates which values have been set (see SYS_INFO_*).
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved system information.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pInfo and/or puMask were NULL pointers.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSystemInfo(HTARGET hTarget, UINT32 uReserved, UINT32 *puMask, SNPS3SystemInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets the extra load flags passed during an ELF load.
///
///  @param		hTarget				Handle to target.
///	 @param		puExtraLoadFlags	Pointer to UINT64 where extra load flags will be stored.
///                             	See SNPS3_XLF_ENABLE_LV2_EXCEPTION_HANDLER etc.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved extra load flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puExtraLoadFlags was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetExtraLoadFlags(HTARGET hTarget, UINT64* puExtraLoadFlags);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Set extra load flags.
///
///  @param		hTarget				Handle to target.
///	 @param		uFlags				New flags to set. See SNPS3_XLF_ENABLE_LV2_EXCEPTION_HANDLER etc.
///  @param		uMask				Mask which indicates which values have been set/unset.
///                             	Use SNPS3_XLF_ALL_FLAG_MASK for all flags.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set extra load flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetExtraLoadFlags(HTARGET hTarget, UINT64 uFlags, UINT64 uMask);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the SDK version of the target from Target Manager.
///             In 6 byte hexadecimal form, first 2 bytes
///             are major version number, second 2 bytes are minor version number,
///             final 2 bytes are fix version number.
///
///	 @param		hTarget			Handle to target.
///	 @param		puSDKVersion	Pointer to UINT64 where SDK version number will be written.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved SDK version.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puSDKVersion was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSDKVersion(HTARGET hTarget, UINT64* puSDKVersion);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the CP version. Read as 6 hexadecimal digits, where (from left to right): first 2 digits
///             are major version number, second 2 digits are minor version number, final 2 digits are fix version number.
///
///	 @param		hTarget			Handle to target.
///	 @param		puCPVersion		Pointer to UINT64 where CP version number will be written.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)		: Any SDK.\n
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved CP version.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCPVersion was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetCPVersion(HTARGET hTarget, UINT64* puCPVersion);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get the list light-weight conditional variables associated with the given process.
///             Call this function passing NULL for pBuffer to get the number of light-weight conditional variables. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget		Handle to target.
///  @param     uProcessID	Process ID.
///  @param     puCount		[in] Size of buffer.\n
///             			[out] Number of light-weight conditional variables for which space is required, 
///             			or if buffer is big enough, number written to buffer.
///  @param     pBuffer		Buffer to hold the set of light-weight conditional variables.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved light-weight conditional variables list or number of light-weight conditional variables.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount was too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetLightWeightConditionalList(HTARGET hTarget, UINT32 uProcessID, UINT32 *puCount, UINT32 *pBuffer);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3LWConditionalAttr
///  @brief     Lightweight conditional attributes.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3LWConditionalAttr
{
	char	aName[8];
};
typedef struct SNPS3LWConditionalAttr SNPS3LWConditionalAttr;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3LWConditionalInfo
///  @brief     Lightweight conditional information.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3LWConditionalInfo
{
	UINT32					uLWCondID;
	SNPS3LWConditionalAttr	Attribute;
	UINT32					uLWMutexID;
	UINT32					uWaitThreadsNum;
	UINT32					uWaitAllThreadsNum;
	UINT64					aWaitThreadIDs[];
};
typedef struct SNPS3LWConditionalInfo SNPS3LWConditionalInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get information about the given light-weight conditional variable.
///             Call this function with pInfo = NULL to get the required buffer size.
///             To get the light-weight conditional variable information, set pInfo to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget	     Handle to target.
///  @param     uProcessID   Process ID.
///  @param     uLWCondVarID Light-weight conditional variable ID.
///  @param     puBufferSize [in] Size of the buffer.\n
///             			 [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pInfo        Pointer to a buffer holding the information about
///                          the light-weight conditional variable.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved light-weight conditional variable information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetLightWeightConditionalInfo(HTARGET hTarget, UINT32 uProcessID, UINT32 uLWCondVarID, UINT32 *puBufferSize, SNPS3LWConditionalInfo *pInfo);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3UserMemoryStats
///  @brief     User memory statistics.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3UserMemoryStats
{
	UINT32 uCreatedSharedMemorySize;
	UINT32 uAttachedSharedMemorySize;
	UINT32 uProcessLocalMemorySize;
	UINT32 uProcessLocalTextSize;
	UINT32 uPRXTextSize;
	UINT32 uPRXDataSize;
	UINT32 uMiscMemorySize;
};
typedef struct SNPS3UserMemoryStats SNPS3UserMemoryStats;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief    Get the memory usage statistics for a process.
///
///  @param	   hTarget			Handle to target.
///	 @param	   uProcessID		ID of process to get statistics for.
///  @param	   pStats			Pointer to SNPS3UserMemoryStats() structure to receive data.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return   \link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved user memory statistics.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pStats was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetUserMemoryStats(HTARGET hTarget, UINT32 uProcessID, SNPS3UserMemoryStats* pStats);

//////////////////////////////////////////////////////////////////////////////
/// @enum	SyncPrimitiveType
/// @brief	Synchronisation primitive types.
//////////////////////////////////////////////////////////////////////////////
typedef enum SyncPrimitiveType {
	/// Mutex
	SPT_MUTEX,
	/// Conditional variable
	SPT_CONDITIONAL_VARIABLE,
	/// Read/write lock
	SPT_RW_LOCK,
	/// Lightweight mutex
	SPT_LW_MUTEX,
	/// Event queue
	SPT_EVENT_QUEUE,
	/// Semaphore
	SPT_SEMAPHORE,
	/// Lightweight conditional variable
	SPT_LW_CONDITIONAL_VARIABLE,
	/// Event flags
	SPT_EVENT_FLAG,
	/// Total number of sync primitive types
	SPT_NUMBER_OF_TYPES // Always the last!
} SyncPrimitiveType;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get a summary of the number of synchronisation primitives in use.
///             Call this function passing NULL for puCounts to get the maximum number of counts to be written.
///             Allocate a block of size (*puBufferSize * sizeof(UINT32)) and call this function again 
///             passing in a pointer (puCounts) to the allocated buffer.
///
///  @param     hTarget        Handle to target.
///  @param     uProcessID     Process ID.
///  @param     puBufferSize   [in] Maximum number of sync primitive counts to write to buffer.\n
///                            [out] Number of counts written to buffer (unless puCounts is NULL, where it will be maximum possible number of counts).
///  @param     puCounts       Address of an array of UINTs to store the 
///                            number of each kind of primitive.\n
///             			   Values are:\n
///             			   puCounts[0] = Mutex count.\n
///             			   puCounts[1] = Conditional variable count.\n
///             			   puCounts[2] = Read/Write lock count.\n
///             			   puCounts[3] = Lightweight mutex count.\n
///             			   puCounts[4] = Event queue count.\n
///             			   puCounts[5] = Semaphore count.\n
///             			   puCounts[6] = Lightweight conditional variable count.\n
///             			   puCounts[7] = Event flag count (sdk 2.7.0 and above).\n
///             			   SyncPrimitiveType() can be used as an index. Be aware that values always 
///             			   come in the same order, e.g if puBufferSize = 2, puCounts will always contain mutex and conditional variable counts.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.8.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved synchronisation primitive counts or maximum number of synchronisation primitive counts.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was a NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///	
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetSyncPrimitiveCountsEx(HTARGET hTarget, UINT32 uProcessID, UINT32* puBufferSize, UINT32* puCounts);

//////////////////////////////////////////////////////////////////////////////
///
///	 @brief		Sets the default elf load priority for the target.
///
///  @param     hTarget			Handle to target.
///  @param		uPriority		Priority value to set.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set default elf load priority.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetDefaultLoadPriority(HTARGET hTarget, UINT32 uPriority);

//////////////////////////////////////////////////////////////////////////////
///
///	 @brief		Gets the default elf load priority for the target.
///
///  @param     hTarget			Handle to target.
///  @param		pPriority		Pointer to UINT32 where value will be stored.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved default elf load priority.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pPriority was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDefaultLoadPriority(HTARGET hTarget, UINT32* pPriority);

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3GamePortIPAddressData
/// @brief	Contains game port information.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3GamePortIPAddressData
{
	UINT32 uReturnValue;
	UINT32 uIPAddress;
	UINT32 uSubnetMask;
	UINT32 uBroadcastAddress;
};
typedef struct SNPS3GamePortIPAddressData SNPS3GamePortIPAddressData;

//! Default ethernet device.
#define SNPS3_DEVICE_ETH0	"eth0"

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Fetches the IP address data used by the game on the target.
///             If the target returns success but a zero IP, this function would previously fail with SN_E_DECI_ERROR.
///             This behaviour changed in 4.1.0 version, and now returns SN_S_OK with the zero IP.
///             Note that a connection to the target is required.
///             To retrieve the IP address/fqdn used by TM to connect to the target,
///             refer to SNPS3GetConnectionInfo().
///
///  @param     hTarget			Handle to target.
///	 @param		szDeviceName	Name of device target is connected on. Set to NULL - currently default of "eth0" is always used.
///  @param		pData			Pointer to SNPS3GamePortIPAddressData() structure to hold data.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved game port IP address data.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_DECI_ERROR - Debug agent returned an error.\n
///             SN_E_BAD_PARAM - pData was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///             SN_E_NOT_CONNECTED - Not connected to target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetGamePortIPAddrData(HTARGET hTarget, const char* szDeviceName, SNPS3GamePortIPAddressData* pData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		If the target is in single nic mode this returns the same value as SNPS3GetGamePortIPAddrData(). 
///             If the target is in dual nic mode this returns the IP address used for host file serving if file serving through the game port is 
///             selected (this IP is set via the XMB, through the Connection Settings for Debug option in debug settings).
///             If the target returns success but a zero IP, this function would previously fail with SN_E_DECI_ERROR.
///             This behaviour changed in 4.1.0 version, and now returns SN_S_OK with the zero IP.
///
///  @param     hTarget			Handle to target.
///	 @param		szDeviceName	Name of device target is connected on. Set to NULL - currently default of "eth0" is always used.
///  @param		pData			Pointer to SNPS3GamePortIPAddressData() structure to hold data.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved game port IP address data.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_DECI_ERROR - Debug agent returned an error.\n
///             SN_E_BAD_PARAM - pData was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.\n
///             SN_E_NOT_CONNECTED - Not connected to target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetGamePortDebugIPAddrData(HTARGET hTarget, const char* szDeviceName, SNPS3GamePortIPAddressData* pData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Continues all threads in a specified thread group.
///
///  @param     hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///  @param		uThreadGroupID	Thread group ID.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully continued SPU thread group.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_ERROR - Internal error.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SPUThreadGroupContinue(HTARGET hTarget, UINT32 uProcessID, UINT64 uThreadGroupID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Queue the given file for download to the host.
///             Note:	This only initiates the download. To check progress, success or failure,
///             		monitor target events (see SNPS3RegisterFTPEventHandler() for more information).
///				Alternatively, use SNPS3WaitForFileTransfer() to check the result.
///
///  @param     hTarget        Handle to target.
///  @param     pSource        Source path. UTF-8 null-terminated string.
///  @param     pDest          Destination path. UTF-8 null-terminated string.
///  @param		pTXID		   Pointer to UINT32 to store transfer ID. Can be NULL.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated file download.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pSource or pDest was NULL pointer or invalid path.\n
///             SN_E_DATA_TOO_LONG - pSource or pDest exceeded maximum path length for system (1024 for PS3, 260 for windows PC).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3DownloadFile(HTARGET hTarget, const char* pSource, const char* pDest, UINT32* pTXID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Queue the contents of the directory for download (does not create the containing directory).
///             Note:	This only initiates the download. To check progress, success or failure,
///             		monitor target events (see SNPS3RegisterFTPEventHandler() for more information).
///				Alternatively, use SNPS3WaitForFileTransfer() to check the result.
///
///  @param     hTarget        Handle to target.
///  @param     pSource        Source directory path. UTF-8 null-terminated string.
///  @param     pDest          Destination directory path. UTF-8 null-terminated string.
///  @param		pLastTXID	   The ID of the last file to be downloaded (to monitor in callback).
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated directory contents download.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - One or more of pSource, pDest and pLastTXID was NULL pointer.\n
///             SN_E_DATA_TOO_LONG - pSource or pDest exceeded maximum path length for system (1024 for PS3, 260 for windows PC).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///             
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3DownloadDirectory(HTARGET hTarget, const char* pSource, const char* pDest, UINT32* pLastTXID);

////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Unregister from file transfer notification events.
///
///  @param		hTarget			Handle to target.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered from file transfer events.\n
///             SN_S_NO_ACTION - No ftp callback function registered.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3CancelFTPEvents(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Gets statistics for single file on target.
///
///  @param		hTarget			Handle to target.
///  @param		pFile			Path of file (must be full path, even if file at root). UTF-8 null-terminated string.
///  @param		pEntry			Pointer to structure to hold information on file.
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved file statistics.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pFile or pEntry NULL pointer, or path to file invalid.\n
///             SN_E_ERROR - File does not exist.\n
///				SN_E_FILE_ERROR - File does not exist.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StatTargetFile(HTARGET hTarget, const char* pFile, SNPS3DirEntry* pEntry);

////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Gets statistics for single file on target.
///
///  @param		hTarget			Handle to target.
///  @param		pFile			Path of file (must be full path, even if file at root). UTF-8 null-terminated string.
///  @param		pEntry			Pointer to structure to hold information on file.
///  @param		pTZ				Pointer to structure to hold target timezone/dst settings (can be null).
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved file statistics.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pFile or pEntry NULL pointer, or path to file invalid.\n
///             SN_E_ERROR - File does not exist.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StatTargetFileEx(HTARGET hTarget, const char* pFile, SNPS3DirEntryEx* pEntry, SNPS3TargetTimezone* pTZ);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Upload all files in a directory (uploads the containing directory).
///             Note:	This only initiates the upload. To check progress, success or failure,
///             		monitor target events (see SNPS3RegisterFTPEventHandler() for more information).
///				Alternatively, use SNPS3WaitForFileTransfer() to check the result.
///
///  @param     hTarget				  Handle to target.
///  @param     pSource				  Source. UTF-8 null-terminated string.
///  @param     pDestination          Destination. UTF-8 null-terminated string.
///  @param		pLastTXID			  Pointer to transfer ID of last file to be uploaded.
///                             	  Set the TXID value to TXID_FORCE_FLAG (0xffffffff) on input to force the transfer
///		                              (ie. bypass the timestamp/size check)
///
///  @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated directory upload.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - One or more of pSource, pDest, pLastTXID was NULL pointer, or source and/or 
///                              destination path was too long or invalid.\n
///             SN_E_DATA_TOO_LONG - pSource or pDest exceeded maximum path length for system (1024 for PS3, 260 for windows PC).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///	
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UploadDirectory(HTARGET hTarget, const char* pSource, const char* pDestination, UINT32* pLastTXID);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Terminate the given game process.
///             See SDK sample in (SDK LOCATION)\\samples\\common\\gcmutil\\exit_callback.cpp for 
///             how to deal with terminate signal on target.
///
///  @param     hTarget        Handle to target.
///  @param     uProcessID     Process ID.
///  @param     uTimeout       Timeout (ms).
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.2.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.2.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully terminated game process.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3TerminateGameProcess(HTARGET hTarget, UINT32 uProcessID, UINT32 uTimeout);

#pragma pack(push, 1)

//////////////////////////////////////////////////////////////////////////////
/// @enum	SNPS3PPUThreadStatus
/// @brief	PPU thread status.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3PPUThreadStatus
{
	UINT64 u64ThreadID;
	/// Status (see defines SNPS3_PPU_IDLE to SNPS3_PPU_DELETED)
	UINT32 uThreadStatus;
};
typedef struct SNPS3PPUThreadStatus SNPS3PPUThreadStatus;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3SPUThreadGroupStatus
/// @brief	SPU thread group status.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3SPUThreadGroupStatus
{
	UINT32 uThreadGroupID;
	/// State (see defines SNPS3_SPU_NOT_CONFIGURED to SNPS3_SPU_STOPPED) 
	UINT32 uThreadGroupStatus;
};
typedef struct SNPS3SPUThreadGroupStatus SNPS3SPUThreadGroupStatus;

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3ProcessTreeBranch
/// @brief	Single branch of process tree (one process)	.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3ProcessTreeBranch
{
	UINT32 uProcessID;
	/// See ESNPS3PROCESSSTATUS.
	UINT32 uProcessStatus;
	UINT32 uPPUThreadCount;
	UINT32 uSPUThreadGroupCount;
	UINT16 uProcessFlags;
	UINT16 uRawSPU;

	SNPS3PPUThreadStatus *aPPUThreadStatus;
	SNPS3SPUThreadGroupStatus *aSPUThreadGroupStatus;
};
typedef struct SNPS3ProcessTreeBranch SNPS3ProcessTreeBranch;

//////////////////////////////////////////////////////////////////////////////
///
/// @brief		Retrieves the process tree (information on processes, their PPU and SPU threads).
///             Call this function passing NULL for pProcessTree to get the number of processes. 
///             Allocate a block of size (*puProcessCount * sizeof(SNPS3ProcessTreeBranch)) and call this function again 
///             passing in a pointer to the allocated buffer.\n
///             Only instances of SNPS3ProcessTreeBranch() struct are stored in buffer
///             pProcessTree points to. Data which its members aPPUThreadStatus and aSPUThreadGroupStatus point to
///             is stored in temporary memory and will be destroyed on next TMAPI function call.
///             Note that this function only returns processes launched via the debug agent (for example TM/TMAPI/Debugger/Tuner).
///             To get processes launched via XMB, use the SNPS3ProcessList() and SNPS3ProcessInfo() functions.
///
/// @param		hTarget			Handle to target.
/// @param		puProcessCount	[in] Number of instances of SNPS3ProcessTreeBranch() buffer can hold.\n
///                             [out] Number of instances of SNPS3ProcessTreeBranch() 
///                             	  for which space is required, or if buffer is big enough, number written to buffer.
/// @param		pProcessTree	Pointer to a buffer to hold the process tree 
///                             information (only SNPS3ProcessTreeBranch() structs stored in buffer).
///
/// @note		 Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved process tree or number of processes.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puProcessCount is NULL pointer.\n
///             SN_E_OUT_OF_MEM - Buffer pointed to by pProcessTree is too small (based on size defined in puProcessCount).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetProcessTree(HTARGET hTarget, UINT32 *puProcessCount, SNPS3ProcessTreeBranch *pProcessTree);

///////////////////////////////////////////////////////////////////////////////
///
/// @brief		Get MAC address of gameport of target.
///
/// @param		hTarget			Handle to target.
/// @param		ppszMacAddress	Pointer to a const char* to receive the MAC address.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	@return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved MAC address.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - ppszMacAddress was NULL pointer.\n
///             SN_E_DECI_ERROR - Debug agent returned an error.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMacAddress(HTARGET hTarget, char **ppszMacAddress);

// these values must correspond to the IDs TM server uses for timeout change operation

//////////////////////////////////////////////////////////////////////////////
/// @enum   SNPS3_TM_TIMEOUT
/// @brief	Target manager timeout type
//////////////////////////////////////////////////////////////////////////////

enum SNPS3_TM_TIMEOUT
{
	/// Default timeout, this timeout will be used for all API functions which contact the target
	DEFAULT_TIMEOUT = 0,
	/// Wait between initiating reset and attempting to connect to the target
	RESET_TIMEOUT,
	/// Connect timeout, maximum amount of time spent attempting to connect (multiple connection attempts will occur in this time)
	CONNECT_TIMEOUT,
	/// Timeout for loading process on target
	LOAD_TIMEOUT,
	/// Timeout for connecting to target to get its status
	GET_STATUS_TIMEOUT,
	/// Maximum time following reset where if target is not connected reset has failed
	RECONNECT_TIMEOUT,
	/// Timeout for functions which contact the target via the game port (generally file serving)
	GAMEPORT_TIMEOUT,
	/// Timeout when processes exited using 'game exit' option in debugger
	GAMEEXIT_TIMEOUT
};

///////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets the various TM timeouts.
///             The IDs in the pTimeoutIDs array
///             correspond to the values in the puTimeoutValues array.\n
///             For example, if pTimeoutIDs[2] = CONNECT_TIMEOUT, puTimeoutValues[2] will
///             be the connect timeout value.
///
/// @param      hTarget			Handle to target.
/// @param		uNoTimeouts		Number of timeouts to be changed.
/// @param		pTimeoutIDs		Array of IDs of timeouts to be changed.
/// @param		puTimeoutValues	Array of new values for timeouts.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set timeouts.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pTimeoutIDs or puTimeoutValues was NULL, or invalid ID in value array (DEH cannot set RECONNECT_TIMEOUT, 
///             	DEX cannot set GAMEPORT_TIMEOUT).
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetTimeouts(HTARGET hTarget, UINT32 uNoTimeouts, SNPS3_TM_TIMEOUT *pTimeoutIDs, UINT32 *puTimeoutValues);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Gets the TM timeouts.
///             Values in auTimeoutIDs and auTimeoutValues arrays correspond.\n
///             For example, if pTimeoutIDs[2] = CONNECT_TIMEOUT, puTimeoutValues[2] will
///             be the connect timeout value.\n
///             Call this function passing NULL for auTimeoutIDs or auTimeoutValues (or both) to get the number of timeout IDs. 
///             Allocate blocks of size (*puNoTimeouts * sizeof(SNPS3_TM_TIMEOUT)) and (*puNoTimeouts * sizeof(UINT32))  
///             and call this function again passing in pointers to the allocated buffers.
///
/// @param		hTarget				Handle to target.
/// @param		puNoTimeouts		Pointer to UINT32 contain number of timeouts (if next params are NULL this will be set).
/// @param		auTimeoutIDs		Pointer to buffer to contain array of timeout ids.
/// @param		auTimeoutValues		Pointer to buffer to contain array of timeout values.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved timeouts or number of timeout IDs.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puNoTimeouts was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Buffers too small (based on size defined in puNoTimeouts).
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetTimeouts(HTARGET hTarget, UINT32 *puNoTimeouts, SNPS3_TM_TIMEOUT *auTimeoutIDs, UINT32 *auTimeoutValues);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Gets the core dump flags of the target.
///
/// @param		hTarget				Handle to target.
/// @param		pu64Flags			Pointer to UINT64 where flags will be written.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved core dump flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - u64Flags was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetCoreDumpFlags(HTARGET hTarget, UINT64 *pu64Flags);

/// @brief	Flag to indicate storing coredump in /dev_ms.
const UINT64 SNPS3TM_CORE_DUMP_TO_DEV_MS			= 0x01;
/// @brief	Flag to indicate storing coredump in /app_home.
const UINT64 SNPS3TM_CORE_DUMP_TO_APP_HOME			= 0x02;
/// @brief	Flag to indicate storing coredump in /dev_usb.
const UINT64 SNPS3TM_CORE_DUMP_TO_DEV_USB			= 0x04;
/// @brief	Flag to indicate storing coredump in /dev_hdd0.
const UINT64 SNPS3TM_CORE_DUMP_TO_DEV_HDD0			= 0x08;
/// @brief	Flag to prevent core dumps on PPU exceptions.
const UINT64 SNPS3TM_CORE_DUMP_DISABLE_PPU_EXCEPTION_DETECTION	= 0x0080000000000000;
/// @brief	Flag to prevent core dumps on SPU exceptions.
const UINT64 SNPS3TM_CORE_DUMP_DISABLE_SPU_EXCEPTION_DETECTION	= 0x0040000000000000;
/// @brief	Flag to prevent core dumps on RSX exceptions.
const UINT64 SNPS3TM_CORE_DUMP_DISABLE_RSX_EXCEPTION_DETECTION	= 0x0020000000000000;
/// @brief	Flag to prevent core dumps on FOOT SWITCH exceptions.
const UINT64 SNPS3TM_CORE_DUMP_DISABLE_FOOT_SWITCH_DETECTION	= 0x0010000000000000;
/// @brief	Flag to prevent core dump including memory details.
const UINT64 SNPS3TM_CORE_DUMP_DISABLE_MEMORY_DUMP				= 0xD0000000;
/// @brief	Restart process after core dump.
const UINT64 SNPS3TM_CORE_DUMP_ENABLE_RESTART_PROCESS			= 0x8000;
/// @brief  Continue running core dump handler after coredump.
const UINT64 SNPS3TM_CORE_DUMP_ENABLE_KEEPRUNNING_HANDLER		= 0x2000;

/////////////////////////////////////////////////////////////////////////////////
///	
/// @brief		Sets the core dump flags of the target.
///
/// @param		hTarget				Handle to target.
/// @param		u64Flags			Flags to set. See SNPS3TM_CORE_DUMP_TO_*.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set core dump flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetCoreDumpFlags(HTARGET hTarget, UINT64 u64Flags);

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Set connection parameters for a specific target.
///
/// @param		hTarget				Handle to target.
/// @param		pConnection			Pointer to a \link TMAPI_TCPIP_CONNECT_PROP TMAPI_TCPIP_CONNECT_PROP\endlink structure containing information
///                             	on connection.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set connection information.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM		Pointer to structure was null.\n
///             SN_E_CONNECTED		Already connected.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetConnectionInfo(HTARGET hTarget, TMAPI_TCPIP_CONNECT_PROP *pConnection);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Stops all threads in a specified SPU thread group.
///
///  @param     hTarget				Handle to target.
///  @param		uProcessID			Process ID.
///  @param		uThreadGroupID		Thread group ID.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully stopped SPU thread group.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SPUThreadGroupStop(HTARGET hTarget, UINT32 uProcessID, UINT64 uThreadGroupID);

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Set file serving case sensitivity for PC (not target, which is always case sensitive).
///
/// @param		hTarget				Handle to target.
/// @param		bOn					True to turn case sensitivity on.
/// @param		pbOldSetting		Optional pointer to bool to receive previous case sensitivity setting. Can be NULL.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set file serving case-sensitivity.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetCaseSensitiveFileServing(HTARGET hTarget, BOOL bOn, BOOL *pbOldSetting);

/// @brief	Flag to enable file create events (cellFsOpen function).
const UINT SNPS3_FS_MASK_CREATE = (1<<0);
/// @brief	Flag to enable file close events (cellFsClose function).
const UINT SNPS3_FS_MASK_CLOSE = (1<<2);
/// @brief	Flag to enable file read events (cellFsRead function).
const UINT SNPS3_FS_MASK_READ = (1<<3);
/// @brief	Flag to enable file write events (cellFsWrite function).
const UINT SNPS3_FS_MASK_WRITE = (1<<4);
/// @brief	Flag to enable file seek events (cellFsLseek function).
const UINT SNPS3_FS_MASK_SEEK = (1<<5);
/// @brief	Flag to enable file delete events (cellFsUnlink function).
const UINT SNPS3_FS_MASK_DELETE = (1<<6);
/// @brief	Flag to enable file rename events (cellFsRename function).
const UINT SNPS3_FS_MASK_RENAME = (1<<7);
/// @brief	Flag to enable file set attribute events (cellFsChMod function).
const UINT SNPS3_FS_MASK_SETATTR = (1<<8);
/// @brief	Flag to enable file get attribute events (cellFsStat, cellFsFStat function).
const UINT SNPS3_FS_MASK_GETATTR = (1<<9);
/// @brief	Flag to enable file set time events (cellFsUtime function).
const UINT SNPS3_FS_MASK_SETTIME = (1<<10);
/// @brief	Flag to enable make directory events (cellFsMkdir function).
const UINT SNPS3_FS_MASK_MKDIR = (1<<11);
/// @brief	Flag to enable remove directory events (cellFsRmdir function).
const UINT SNPS3_FS_MASK_RMDIR = (1<<12);
/// @brief	Flag to enable directory open events (cellFsOpendir function).
const UINT SNPS3_FS_MASK_OPENDIR = (1<<13);
/// @brief	Flag to enable directory close events (cellFsClosedir function).
const UINT SNPS3_FS_MASK_CLOSEDIR = (1<<14);
/// @brief	Flag to enable directory read events (cellFsReaddir function).
const UINT SNPS3_FS_MASK_READDIR = (1<<15);
/// @brief	Flag to enable file truncate events (cellFsTruncate, cellFsFtruncate functions).
const UINT SNPS3_FS_MASK_TRUNCATE = (1<<16);
/// @brief	Flag to enable file get attribute events (cellFsFStat function).
const UINT SNPS3_FS_MASK_FGETATTR64 = (1<<17);
/// @brief	Flag to enable file get attribute events (cellFsStat function).
const UINT SNPS3_FS_MASK_GETATTR64 = (1<<18);
/// @brief	Flag to enable all file serving events.
const UINT SNPS3_FS_MASK_ALL = SNPS3_FS_MASK_CREATE | SNPS3_FS_MASK_CLOSE | SNPS3_FS_MASK_READ | SNPS3_FS_MASK_WRITE 
							| SNPS3_FS_MASK_SEEK | SNPS3_FS_MASK_DELETE | SNPS3_FS_MASK_RENAME | SNPS3_FS_MASK_SETATTR 
							| SNPS3_FS_MASK_GETATTR | SNPS3_FS_MASK_SETTIME | SNPS3_FS_MASK_MKDIR | SNPS3_FS_MASK_RMDIR
							| SNPS3_FS_MASK_OPENDIR | SNPS3_FS_MASK_CLOSEDIR | SNPS3_FS_MASK_READDIR | SNPS3_FS_MASK_TRUNCATE 
							| SNPS3_FS_MASK_FGETATTR64 | SNPS3_FS_MASK_GETATTR64;

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Set file serving event flags.
///
/// @param		hTarget				      Handle to target.
/// @param		uFileServingEventFlags    See SNPS3_FS_MASK_CREATE etc.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: All SDK Versions.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: All SDK Versions.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: All SDK Versions.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set file serving event flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetFileServingEventFlags(HTARGET hTarget, UINT64 uFileServingEventFlags);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Gets file serving event flags for target.
///
/// @param		hTarget				       Handle to target.
/// @param		puFileServingEventFlags	   Pointer to UINT64 to hold file serving event flags. See SNPS3_FS_MASK_CREATE etc.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: All SDK Versions.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: All SDK Versions.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: All SDK Versions.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved file serving event flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puFileServingEventFlags was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetFileServingEventFlags(HTARGET hTarget, UINT64 *puFileServingEventFlags);


//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3VirtualMemoryArea
/// @brief	Virtual memory area data.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3VirtualMemoryArea
{
		UINT64 uAddress;			
		UINT64 uFlags;				
		UINT64 uVSize;				
		UINT64 uOptions;			
		UINT64 uPageFaultPPU;
		UINT64 uPageFaultSPU;
		UINT64 uPageIn;
		UINT64 uPageOut;
		UINT64 uPMemTotal;
		UINT64 uPMemUsed;
		UINT64 uTime;
		UINT64 uPageCount;
		UINT64 *puPages;
};
typedef struct SNPS3VirtualMemoryArea SNPS3VirtualMemoryArea;

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Get information on virtual memory areas.
///             Call this function with pBuffer = NULL to get the required buffer size.
///             To get the virtual memory areas information, set pBuffer to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.\n
///             The buffer is an array of SNPS3VirtualMemoryArea() structures followed by the page data.
///             The structures contain pointers to within the buffer where the page data is stored.
///
/// @param		hTarget				Handle to target.
/// @param		uPID				ProcessID.
/// @param		bStatsOnly			Set to true if page data is not desired.
/// @param		puAreaCount			On success, contains number of virtual memory areas.
/// @param		puBufferSize		[in] Size of buffer.
///                             	[out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
/// @param		pBuffer				Buffer to contain virtual memory information.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved virtual memory information or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM		puAreaCount and/or puBufSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM		Size of buffer defined in puBufferSize was too small.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetVirtualMemoryInfo(HTARGET hTarget, UINT32 uPID, BOOL bStatsOnly, UINT32 *puAreaCount, UINT32 *puBufferSize, BYTE *pBuffer);

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Flash target with the specified PS3ToolUpdater.self and PS3UPDAT.PUP.
///
/// @param		hTarget				Handle to target.
/// @param		pszUpdaterTool		Path to PS3ToolUpdater.self (UTF-8 null-terminated string) or NULL to use built in updater tool. 
/// @param		pszFlashImage		Path to PS3UPDAT.PUP file. UTF-8 null-terminated string.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.6.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 1.6.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully flashed target.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszFlashImage was NULL or PUP incompatible with target type.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3FlashTarget(HTARGET hTarget, const char* pszUpdaterTool, const char* pszFlashImage);

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Set loop point in SPU thread (replaces instruction with looping instruction, effectively stalling thread).
///             SNPS3ClearSPULoopPoint() will replace looping instruction with previously stored instruction.\n
///             The thread group must be stopped when SPU loop points are set/cleared.
///
/// @param		hTarget				Handle to target.
/// @param		uProcessID			Process ID.
/// @param		uThreadID			SPU thread ID.
/// @param		uAddress			Address to set loop point.
/// @param		bCurrentPC			If equals true then address is ignored and loop point is set at current PC address.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set SPU loop point.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_TM_COMMS_ERR  - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetSPULoopPoint(HTARGET hTarget, UINT32 uProcessID, UINT64 uThreadID, UINT32 uAddress, BOOL bCurrentPC);

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Clear loop point from SPU thread (replaces looping instruction with previously stored instruction).
///             The thread group must be stopped when SPU loop points are set/cleared.
///
/// @param		hTarget				Handle to target.
/// @param		uProcessID			Process ID.
/// @param		uThreadID			SPU thread ID.
/// @param		uAddress			Address to clear loop point from.
/// @param		bCurrentPC			If equals true then address is ignored and loop point is cleared from current PC address.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully cleared SPU loop point.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ClearSPULoopPoint(HTARGET hTarget, UINT32 uProcessID, UINT64 uThreadID, UINT32 uAddress, BOOL bCurrentPC);

/////////////////////////////////////////////////////////////////////////////////////
///
/// @brief	    Trigger a core dump of the given process.
///             When the core dump is complete, a DBG target event 
///             \link SNPS3_EVENTNOTIFY_CORE_DUMP_COMPLETE SNPS3_EVENTNOTIFY_CORE_DUMP_COMPLETE\endlink 
///             will be sent.
///             To receive this event, register for target events using SNPS3RegisterTargetEventHandler()
///             and call SNPS3Kick() to receive notifications.
///
/// @param		hTarget				Handle to target.
/// @param		uProcessID			Process ID.
/// @param		uUserData1			User data to be written to core dump file.
/// @param		uUserData2			User data to be written to core dump file.
/// @param		uUserData3			User data to be written to core dump file.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully triggered core dump.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3TriggerCoreDump(HTARGET hTarget, UINT32 uProcessID, UINT64 uUserData1, UINT64 uUserData2, UINT64 uUserData3);

//////////////////////////////////////////////////////////////////////////////
/// @enum		SNPS3_DISPLAY_MODE
/// @brief	Display mode (used for VRAM capture).
//////////////////////////////////////////////////////////////////////////////

enum SNPS3_DISPLAY_MODE {
	X8R8G8B8,
	X8B8G8R8,
	R16G16B16X16
};

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3VRAMInfo
/// @brief	VRAM information.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3VRAMInfo
{
	/// Address at which to put breakpoint in order to stop the process immediately before/after 'flip'.
	UINT64		uBPAddress;
	/// Address of a pointer that points to the image in memory.
	UINT64		uTopAddressPointer;
	/// Width of image.
	UINT32		uWidth;
	/// Height of image.
	UINT32		uHeight;
	/// Image pitch (as described in CellGCMSurface) - in bytes.
	UINT32		uPitch;
	/// Image colour settings (0 = X8R8G8B8, 1 = X8B8G8R8, 2 = R16G16B16X16).
	BYTE		colour;
};
typedef struct SNPS3VRAMInfo SNPS3VRAMInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets VRAM information for given process.
///             Note that due to a hardware limitation, it is necessary to wait for
///             at least 500ms after loading a process before calling this function, otherwise
///             it will fail.
///             Secondary VRAM information is optional, set pointer to NULL to ignore.
///
///  @param		hTarget			Handle to target.
///  @param		uProcessID		Process ID.
///  @param		pPrimary		Pointer to instance of SNPS3VRAMInfo() to hold primary information.
///  @param		pSecondary		Pointer to instance of SNPS3VRAMInfo() to hold secondary information (can be NULL).
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved VRAM information.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pPrimary was NULL.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetVRAMInformation(HTARGET hTarget, UINT32 uProcessID, SNPS3VRAMInfo *pPrimary, SNPS3VRAMInfo *pSecondary);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Set data access breakpoint.
///             Process must be loaded with \link SNPS3_LOAD_FLAG_ENABLE_DEBUGGING SNPS3_LOAD_FLAG_ENABLE_DEBUGGING\endlink set.
///             All PPU threads in the target process should be stopped before calling this function (see SNPS3ProcessStop()).
///             Note: Only one data access breakpoint can be set per process.
///
///	 @param		hTarget			Handle to target.
///	 @param		uProcessID		Process ID.
///	 @param		uAddr			Address of data to set breakpoint on.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set data access breakpoint.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetDABR(HTARGET hTarget, UINT32 uProcessID, UINT64 uAddr);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Get address of current data access breakpoint.
///             All PPU threads in the target process should be stopped before calling this function (see SNPS3ProcessStop()).
///             Note: There will be only one data access breakpoint per process.
///
///	 @param		hTarget			Handle to target.
///	 @param		uProcessID		Process ID.
///	 @param		puAddr			Pointer to UINT64 where data address will be written.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved data access breakpoint address.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puAddr was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDABR(HTARGET hTarget, UINT32 uProcessID, UINT64 *puAddr);

/////////////////////////////////////////////////////////////////////////////
///
///  @brief		Get further information on error, if available.
///             pQualifier/ppszErrorString can be set to NULL if either value is not of interest.
///
///  @param		pQualifier		Pointer to UINT32 where last error code will be stored.
///  @param		ppszErrorString	Pointer to char pointer where address of last error message will be stored. 
///								UTF-8 null-terminated string.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved error qualifier.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetErrorQualifier(UINT32 *pQualifier, char **ppszErrorString);

// @name <CompressionLevels>
//@{
//! Compression level for retrieving memory.
#define SNPS3_COMPRESSION_LEVEL_NONE					0
#define SNPS3_COMPRESSION_LEVEL_BEST_SPEED              1
#define SNPS3_COMPRESSION_LEVEL_COMPRESSION				9
#define SNPS3_COMPRESSION_LEVEL_DEFAULT					(-1)
//@}

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Gets a range of memory, compressing and decompressing at start/end of communications.
///             Address can be a maximum of 32 bits, for more use SNPS3GetMemory64Compressed().
///
///	 @param		hTarget			Handle to target.
///	 @param		uProcessID		Process ID.
///  @param		uLevel			Compression level - see SNPS3_COMPRESSION_LEVEL_* - can be value between 1 and 9 - 1 being best speed,
///                             9 being best compression.
///                             Default value (-1) will request a compromise between compression and speed.
///	 @param		uAddr			Address of memory to read from.
///  @param		uSize			Size of data to read.
///	 @param		pBuffer			Pointer to buffer where data will be written.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.9.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved memory.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pBuffer was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMemoryCompressed(HTARGET hTarget, UINT32 uProcessID, UINT32 uLevel, UINT32 uAddr, UINT32 uSize, BYTE* pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Gets a range of memory, compressing and decompressing at start/end of communications (improved speed).
///             Address of memory to get can be up to 64 bits long.
///
///	 @param		hTarget			Handle to target.
///	 @param		uProcessID		Process ID.
///  @param		uLevel			Compression level - see SNPS3_COMPRESSION_LEVEL_* - can be value between 1 and 9 - 1 being best speed,
///                             9 being best compression. Default value (-1) will request compromise between compression and speed.
///	 @param		uAddr			Address of memory to read from (max 64 bits).
///  @param		uSize			Size of data to read.
///	 @param		pBuffer			Pointer to buffer where data will be written.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.9.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved memory.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pBuffer was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMemory64Compressed(HTARGET hTarget, UINT32 uProcessID, UINT32 uLevel, UINT64 uAddr, UINT32 uSize, BYTE* pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Sets a breakpoint.
///
///  @param		hTarget			Handle to a target.
///  @param		uUnit			Unit upon which the process runs (see \link PS3TMAPI::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param     uProcessID		Process ID.
///  @param     uThreadID		Thread ID.
///  @param		uAddress		Address of the break point.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set breakpoint.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_UNIT - uUnit was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetBreakPoint(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID, UINT64 uAddress);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Clear a break point.
///
///  @param		hTarget			Handle to a target.
///  @param		uUnit			Unit upon which the process runs (see \link PS3TMAPI::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param     uProcessID		Process ID.
///  @param     uThreadID		Thread ID.
///  @param		uAddress		Address of the break point.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully cleared breakpoint.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_UNIT - uUnit was invalid (see \link PS3TMAPI::ESNPS3UNIT ESNPS3UNIT\endlink).\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ClearBreakPoint(HTARGET hTarget, UINT32 uUnit, UINT32 uProcessID, UINT64 uThreadID, UINT64 uAddress);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		List breakpoints set in process.
///             Call this function passing NULL for au64BPAddress to get the number of breakpoints. 
///             Allocate a block of size (*puBPCount * sizeof(UINT64)) and call this function again,
///             passing in a pointer to the allocated buffer.
///
/// @param		hTarget			Handle to target.
/// @param		uUnit			Unit upon which the process runs (see \link PS3TMAPI::ESNPS3UNIT ESNPS3UNIT\endlink).
/// @param		uPID			Process ID.
/// @param		u64TID			Thread ID. This is only used for SPU breakpoints. If the unit is PPU then this is ignored.
/// @param		puBPCount		[in] Number of breakpoints buffer can hold.\n
///                             [out] Number of breakpoints for which space is required, or if buffer is big enough, number written to buffer.
/// @param		au64BPAddress	Pointer to buffer to contain addresses of breakpoints.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved breakpoint list or number of breakpoints.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_UNIT - uUnit was invalid (see \link PS3TMAPI::ESNPS3UNIT ESNPS3UNIT\endlink).\n
///             SN_E_BAD_PARAM - puBPCount was a NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer defined by puBPCount was smaller than required size.\n
///             SN_E_TM_COMMS_ERR - Error communicating with TM Server.\n
///             SN_E_COMMS_ERR - Error communicating with target. If this return code was received when 
///                              getting SPU breakpoints, the debug agent may need to be updated.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetBreakPoints(HTARGET hTarget, UINT32 uUnit, UINT32 uPID, UINT64 u64TID, UINT32 *puBPCount, UINT64 *au64BPAddress);

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3ControlKeywordEntry
///  @brief     Debug control keyword information. See "DEBUG_THREAD_CTRL_KEYWORD" in the DBGP specification.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3ControlKeywordEntry
{
	UINT32 uMatchConditionFlags;
	char aKeywordString[128];

};
typedef struct SNPS3ControlKeywordEntry SNPS3ControlKeywordEntry;

//////////////////////////////////////////////////////////////////////////////
///  @struct	SNPS3DebugThreadControlInfo
///  @brief     Debug control keyword information. See "DEBUG_THREAD_CTRL_KEYWORD" in the DBGP specification.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3DebugThreadControlInfo
{
	UINT64 uControlFlags;
	UINT32 uNumEntries;
	SNPS3ControlKeywordEntry arrCtrlKeywordEntry[];
};
typedef struct SNPS3DebugThreadControlInfo SNPS3DebugThreadControlInfo;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Get a list of all the debug thread control settings.
///             Call this function with pBuffer = NULL to get the required buffer size.
///             To get the debug thread control settings, set pBuffer to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///             Buffer will contain an instance of SNPS3DebugThreadControlInfo() 
///             (buffer size is variable due to variable array arrCtrlKeywordEntry).
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     puBufferSize	[in] Size of buffer.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param     pBuffer			Pointer to the buffer.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved debug thread control settings or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puBufferSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer defined in puBufferSize is too small.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetDebugThreadControlInfo(HTARGET hTarget, UINT32 uProcessID, UINT32* puBufferSize, SNPS3DebugThreadControlInfo* pBuffer);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Set the debug thread control settings for a process.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param     pInfo			Instance of SNPS3DebugThreadControlInfo() containing new settings.
///  @param		puMaxEntries	Pointer to UINT32 to store maximum number of control keyword entries.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set debug thread control settings.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pInfo or puMaxEntries was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetDebugThreadControlInfo(HTARGET hTarget, UINT32 uProcessID, SNPS3DebugThreadControlInfo* pInfo, UINT32 *puMaxEntries);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Attach to a process.
///             Note: Currently can only attach to a PPU process (which will give access to 
///             any SPU children), thus any unit ID other than PS3_UI_CPU will cause the function to fail.
///
///  @param     hTarget	    Handle to target.
///  @param     uUnitID     Unit upon which the process is running (see \link PS3tmapi.h::ESNPS3UNIT ESNPS3UNIT\endlink).
///  @param     uProcessID  Process ID.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully attached to process.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_UNIT - uUnit was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessAttach(HTARGET hTarget, UINT32 uUnitID, UINT32 uProcessID);

//////////////////////////////////////////////////////////////////////////////
/// @struct	SNPS3MATRange
///	@brief	Information on single range of memory access traps.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3MATRange
{
	/// Start address of MAT range (aligned to 4k)
	UINT32 uStartAddress;
	/// Size of MAT range (aligned to 4k)
	UINT32 uSize;
	/// Byte array of MAT condition for each 4k page in range (see defines of SNPS3_MAT_COND_*)
	BYTE aPageConditions[];
};
typedef struct SNPS3MATRange SNPS3MATRange;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Set memory access trap conditions for a process.
///             Buffer is array of SNPS3MATRange() structures.\n
///             Note: Page size is 4k so start address must be 4k aligned and size must be a multiple of 4k.\n
///             Page condition array contains 1 byte per page.\n
///             See SNPS3_MAT_COND_* defines for possible conditions.
///             Note that a process must have been loaded with extra load flag SNPS3_XLF_ENABLE_MAT (see SNPS3SetExtraLoadFlags)
///             set in order to use memory access traps.
///
///  @param     hTarget	    Handle to target.
///  @param     uProcessID  Process ID.
///  @param		uRangeCount	Number of ranges defined in buffer.
///  @param		uBufSize	Size of pBuf.
///  @param		pBuf		Pointer to buffer containing conditions.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set memory access trap conditions.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pBuf was NULL pointer.\n
///             SN_E_BAD_ALIGN - Page size or start address in MAT data not aligned correctly (4k align).\n
///             SN_E_INSUFFICIENT_DATA - Insufficient data supplied.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetMATConditions(HTARGET hTarget, UINT32 uProcessID, UINT32 uRangeCount, UINT32 uBufSize, BYTE *pBuf);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Get list of address ranges for a process where memory access traps can be set.
///             Buffer is array of SNPS3MATRange() structures where aPageConditions will always be empty.\n
///             Call this function passing NULL for pBuf to get the number of address ranges. 
///             Allocate a block of size (*puRangeCount * sizeof(SNPS3MATRange)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param		puRangeCount 	[in] Total number of ranges buffer can hold (buffer size = puRangeCount * sizeof(SNPS3MATRange)).\n
///                             [out] Number of ranges for which space is required, or if buffer is big enough, number written to buffer.
///	 @param		pBuf			Pointer to buffer containing range information.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved memory access trap ranges or number of ranges.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puRangeCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM- Size of buffer specified in puRangeCount not large enough.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMATRanges(HTARGET hTarget, UINT32 uProcessID, UINT32 *puRangeCount, BYTE *pBuf);

// @name <MatConditions>
//@{
//! Memory access trap condition.
#define SNPS3_MAT_COND_TRANSPARENT  0
#define SNPS3_MAT_COND_WRITE        1
#define SNPS3_MAT_COND_READ_WRITE   2
#define SNPS3_MAT_COND_ERR          3
//@}

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Get current settings of memory access traps for specified address ranges for a process.
///             Input buffer is array of SNPS3MATRange() structures (with empty aPageConditions arrays)
///             specifying what memory ranges to get conditions for.\n
///             Output buffer is also array of SNPS3MATRange() structures where aPageConditions arrays will
///             now be filled with requested information.\n
///             See SNPS3_MAT_COND_* defines for possible conditions.\n
///             Call this function with pBuf = NULL to get the required buffer size.
///             To get the memory access traps information, set pBuf to point to an allocated memory block of size 
///             *puBufferSize, and call the function again.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///	 @param		puRangeCount	[in] Number of ranges requested in pRanges buffer.\n
///                             [out] Number of ranges successfully read.
///  @param		pRanges			Pointer to buffer containing ranges to get conditions for.
///  @param		puBufSize		[in] Size of output buffer pBuf in bytes.\n
///                             [out] Required size of buffer, or if buffer is big enough, bytes written to buffer.
///  @param		pBuf			Pointer to buffer to store ranges and conditions.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved memory access trap conditions or required size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - One or more of puRangeCount, pRanges or puBufSize was NULL pointer.\n
///             SN_E_OUT_OF_MEM- Size of buffer specified in puBufSize not large enough.\n
///             SN_E_BAD_ALIGN - One or more of specified addresses/range sizes was not aligned to 4096 bytes.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMATConditions(HTARGET hTarget, UINT32 uProcessID, UINT32 *puRangeCount, BYTE *pRanges, UINT32 *puBufSize, BYTE *pBuf);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief		Returns array of pointers to instances of \link SNPS3MATRange() SNPS3MATRange()\endlink within the supplied
///             buffer pBuf.
///             pBuf needs to be in the form returned by SNPS3GetMATConditions().
///
///  @param		uRangeCount		Number of ranges in buffer.
///	 @param		pBuf			Pointer to byte buffer containing series of SNPS3MATRange() structures.
///  @param		ppRanges		Pointer to array of SNPS3MATRange() pointers (one for each range).
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved memory access trap pointers.\n
///             SN_E_BAD_PARAM - pBuf or ppRanges was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetMATRangePointers(UINT32 uRangeCount, BYTE *pBuf, SNPS3MATRange **ppRanges);

////////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Shut down TM server.
///             If the function succeeds, communication is fully closed and calling SNPS3CloseTargetComms() is not required.
///             From SDK400, the function shuts down the TM GUI and icon tray as well.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully shut down TM.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TIMEOUT - Timeout (20sec) occurred during the execution of the function - see SNPS3ExitEx().
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3Exit();

////////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Shut down TM server.
///             If the function succeeds, communication is fully closed and calling SNPS3CloseTargetComms() is not required.
///             From SDK400, the function shuts down the TM GUI and icon tray as well.
///
/// @param		nMillisecondTimeout	Millisecond timeout value.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully shut down TM.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TIMEOUT - Timeout occurred during the execution of the function.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ExitEx(const UINT32 nMillisecondTimeout);

////////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Save current TM settings.
///
/// @return		\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully saved TM settings.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SaveSettings();

////////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Get raw SPU Logical IDs.
///             Note: Will return a value for all physical SPUs, even those not allocated to RAW SPUs.
///             These will have an ID of 0xFFFFFFFFFFFFFFFF.
///
///  @param     hTarget			Handle to target.
///  @param     uProcessID		Process ID.
///  @param		pu64LogicalIDs	Pointer to array of 8 64 bit unsigned integers to hold IDs.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved raw SPU logical IDs.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pu64LogicalIDs was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetRawSPULogicalIDs(HTARGET hTarget, UINT32 uProcessID, UINT64 *pu64LogicalIDs);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Enable or disable footswitch notifications (disabled by default).
///
///  @param     hTarget			Handle to target.
///  @param     uEnabled		Set to 0 to disable notifications, non-zero to enable notifications.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully enabled / disabled footswitch notifications.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3FootswitchControl(HTARGET hTarget, UINT32 uEnabled);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Start a file trace for the process.
///
///  @param     hTarget			Handle to target.
///  @param     uPID			Process ID.
///  @param		uSize			Size of the container that will be created for use by the file trace. 1mb is the recommended size.
///  @param		pszFileName		Specifies the location to which the trace file will be written. UTF-8 null-terminated string.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n\n
///             SN_S_OK - Successfully started file trace.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszFileName was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StartFileTrace(HTARGET hTarget, UINT32 uPID, UINT32 uSize, const char *pszFileName);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Stop process file trace.
///
///  @param     hTarget			Handle to target.
///  @param     uPID			Process ID.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully stopped file trace.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StopFileTrace(HTARGET hTarget, UINT32 uPID);

//! Control pad connected.
const BYTE SNPS3_PAD_STATUS_CONNECTED = 0x1;
//! Change of assigned controller port.
const BYTE SNPS3_PAD_STATUS_ASSIGN_CHANGE = 0x2;

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3PadData
///	@brief		Pad playback data.
//////////////////////////////////////////////////////////////////////////////

typedef struct SNPS3PadData {
	/// First 32 bits of timestamp.
	UINT32 ulTimeHi;
	/// Second 32 bits of timestamp.
	UINT32 ulTimeLo;
	/// Reserved.
	UINT32 ulReserved0;
	/// Reserved.
	UINT32 ulReserved1;
	/// Controller port.
	UCHAR  ucPort;
	/// Status of controller port.
	UCHAR  ucPortStatus;
	/// Length of button data.
	UCHAR  ucLength;
	/// Reserved.
	UCHAR  ucReserved2;
	/// Reserved.
	UINT32 ulReserved3;
	/// Button data
	UINT16 arrButtons[24];
} SNPS3PadData;

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Start pad capture on the target (recording all information from the control pad).
///             To receive control pad data, register a pad capture callback function using SNPS3RegisterPadCaptureHandler(). 
///             Instances of SNPS3PadData() will be passed
///             to the callback each time pad data is sent by the target. See SDK CTRLP spec for details.
///             This function will fail if a pad capture has already been started on the target.
///
///  @param     hTarget			Handle to target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully started pad capture.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StartPadCapture(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Stop pad capture on the target.
///
///  @param     hTarget			Handle to target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully stopped pad capture.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StopPadCapture(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Start pad playback on the target.
///             Once pad playback is started, pad capture data can be sent using SNPS3SendPadPlaybackData().
///
///  @param     hTarget			Handle to target
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully started pad playback.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StartPadPlayback(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Stop pad playback on the target.
///
///  @param     hTarget			Handle to target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully stopped pad playback.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3StopPadPlayback(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Register a pad capture callback function.
///             The callback function behaves as other TMAPI callback functions do.\n
///             Data received by the callback will be instances of the SNPS3PadData() structure, sent 
///             each time pad data is sent by the target.\n
///             The length field in the callback will contain the number of instances of the 
///             SNPS3PadData() structure in the buffer.\n
///             SNPS3StartPadCapture() must have been called to receive data on the callback.
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param     hTarget			Handle to target.
///  @param		pfnCallBack		Pointer to callback function.
///  @param		pUserData		Pointer to optional user data to be passed to callback function. Can be NULL.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered pad capture handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pfnCallBack was NULL pointer.\n
///             SN_S_REPLACED - Already registered for pad capture events, previous callback function has been replaced.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterPadCaptureHandler(HTARGET hTarget, TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Unregister a pad capture callback function.
///
///  @param     hTarget			Handle to target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered pad capture handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_S_NO_ACTION - No callback function registered.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UnRegisterPadCaptureHandler(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Send pad capture data to the target for playback.
///
///  @param     hTarget			Handle to target.
///  @param		pData			Pointer to instance of SNPS3PadData() structure to send to the target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully sent pad playback data.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pData was NULL pointer\n
///             SN_E_BUSY - Previous send command not completed.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SendPadPlaybackData(HTARGET hTarget, SNPS3PadData* pData);

/// @brief	Controller information received by target.
const UINT32 SNPS3TM_CTRLP_OK = 0x00000000;
/// @brief	Invalid packet (for example, controller information has been sent when its playback was not being processed).
const UINT32 SNPS3TM_CTRLP_EINVAL = 0x80010002;
/// @brief	Insufficient memory.
const UINT32 SNPS3TM_CTRLP_ENOMEM = 0x80010004;
/// @brief	Already playing.
const UINT32 SNPS3TM_CTRLP_EBUSY = 0x8001000a;
/// @brief	Controller disconnected.
const UINT32 SNPS3TM_CTRLP_ENODEV = 0x8001002d;

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Register a pad playback notification callback function to receive notifications
///             from target when pad playback data is sent via SNPS3SendPadPlaybackData().
///             The callback will receive a UINT32 which will be one of 
///             \link PS3TMAPI::SNPS3TM_CTRLP_OK SNPS3TM_CTRLP_OK\endlink through to 
///             \link PS3TMAPI::SNPS3TM_CTRLP_ENODEV SNPS3TM_CTRLP_ENODEV\endlink.
///             The "Length" parameter passed to the callback is the number of UINT32s in the data, 
///             not the data length in bytes.
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param     hTarget			Handle to target.
///  @param		pfnCallBack		Pointer to callback function.
///  @param		pUserData		Pointer to optional user data to be passed to callback function.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered pad playback handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pfnCallBack was NULL pointer.\n
///             SN_S_REPLACED - Already registered for pad playback events, previous callback function has been replaced.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterPadPlaybackNotificationHandler(HTARGET hTarget, TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Unregister a pad playback notification callback function.
///
///  @param     hTarget			Handle to target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered pad playback handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_S_NO_ACTION - No callback function registered.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UnRegisterPadPlaybackNotificationHandler(HTARGET hTarget);

////////////////////////////////////////////////////////////////////////////////////
///
///  @brief		Clear the TTY cache used for the SAVETTY special fileserving command.
///
///  @param     hTarget			Handle to target.
///
///  @return	\link ApiBase::SNRESULT SNRESULT:\endlink\n
///             SN_S_OK - Successfully cleared TTY cache.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ClearTTYCache(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Gets all file trace events from file created by SNPS3StartFileTrace() function.
///             Events from file will be passed to callback registered by SNPS3RegisterFileTraceHandler().
///
///  @param		hTarget			Handle to target.
///	 @param		pszPath			Path of file trace. UTF-8 null-terminated string.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
/// 
///  @return	\link ApiBase::SNRESULT SNRESULT:\endlink\n
///             SN_S_OK - Successfully retrieved file trace events.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszPath was NULL pointer, or contained invalid path.\n
///             SN_E_FILE_ERROR - Error retrieving file trace events.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessOfflineFileTrace(HTARGET hTarget, const char* pszPath);

///////////////////////////////////////////////////////////////////////////////
///
///  @brief		Register file trace event handler.
///             Trace events will be passed to handler during a file trace
///             or after calling SNPS3ProcessOfflineFileTrace().
///             The callback will only be invoked when SNPS3Kick() is called.
///
///  @param		hTarget			Handle to target.
///  @param		pfnCallBack		Pointer to callback function.
///  @param		pUserData		Optional pointer to user data.
///
///  Callback must be in the form of:
///
/// \code
///  __stdcall void Callback(
///          HTARGET hTarget,       // Handle to target.
///          UINT32 Type,           // SN_EVENT_FILE_TRACE.
///          UINT32 Param,          // Reserved - currently 0.
///          SNRESULT eResult,      // Success or failure of data fetch.
///          UINT32 Length,         // Length of data in event including header.
///          BYTE* Data,            // Pointer to event data - SNPS3_FILE_TRACE_LOG.
///          void* UserData);       // Optional user data.
/// \endcode
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
/// 
///  @return	\link ApiBase::SNRESULT SNRESULT:\endlink\n
///             SN_S_OK - Successfully registered file trace handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pfnCallBack was NULL pointer.\n
///             SN_S_REPLACED - Already registered for file trace events, previous callback function has been replaced.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterFileTraceHandler(HTARGET hTarget, TMAPI_HandleEventCallback pfnCallBack, void *pUserData);

///////////////////////////////////////////////////////////////////////////////
///
///  @brief		Unregister file trace event callback.
///
///  @param		hTarget			Handle to target.
///
///  @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n\n
///             SN_S_OK - Successfully unregistered file trace handler.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_S_NO_ACTION - No callback function registered.
/// 
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UnRegisterFileTraceHandler(HTARGET hTarget);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Fetches a list of user processes running on the specified target.
///             Call this function passing NULL for puBuffer to get the number of user processes. 
///             Allocate a block of size (*puCount * sizeof(UINT32)) and call this function again 
///             passing in a pointer to the allocated buffer.
///
///  @param     hTarget			Handle to target.
///  @param		puCount			[in] Maximum number of IDs buffer can hold.\n
///                             [out] Number of processes for which space is required, 
///                             or if buffer is big enough, number written to buffer.
///  @param		puBuffer		Pointer to a buffer to receive the information.
///                             As a guide, allocate a buffer of 64 * sizeof(UINT32),
///                             passing 64 as the count.
/// 
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 1.0.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.0.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved user process list or number of user processes.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.\n
///             SN_E_OUT_OF_MEM - Size of buffer specified in puCount (in UINT32s) smaller than required.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UserProcessList(HTARGET hTarget, UINT32 *puCount, UINT32 *puBuffer);

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3ScatteredWrite
///	@brief		Details of a write (as part of a scattered write).
//////////////////////////////////////////////////////////////////////////////

struct SNPS3ScatteredWrite
{
	/// Address to write to.
	UINT32 uAddress;
	/// Buffer containing data to write (buffer size is passed to scattered write function)
	BYTE Data[];
};
typedef struct SNPS3ScatteredWrite SNPS3ScatteredWrite;

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Write data to each address in a list of addresses.
///             The data written to each address may be different but the size of the data must always be the same.\n
///             If a write fails, the command finishes and the failed address is written to *puFailedAddress, 
///             while the error code written to *puErrorCode. 
///             All writes previous to the failed write will have completed successfully.
///
///  @param     hTarget			Handle to target.
///  @param		uPID			Process ID.
///  @param		uNumWrites		Number of writes to send.
///	 @param		uWriteSize		Number of bytes to be written on a single write.
///  @param		pWrites			Pointer to series of SNPS3ScatteredWrite() 
///                             structures containing information on writes to be performed.
///  @param		puErrorCode		Pointer to unsigned int where error code of first failed write will be written 
///                             (if there is a failure) - can be NULL.
///  @param		puFailedAddress	Pointer to unsigned int where address of first failed write will be written 
///                             (if there is a failure) - can be NULL.
///		
///	 @note      Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully wrote to memory locations.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pWrites was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ProcessScatteredSetMemory(HTARGET hTarget, UINT32 uPID, UINT32 uNumWrites, UINT32 uWriteSize, SNPS3ScatteredWrite *pWrites, 
													UINT32 *puErrorCode, UINT32 *puFailedAddress);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets RSX profiling flags (note extra load flag SNPS3_XLF_ENABLE_HUD_RSX_TOOLS will need to be specified).
///             For RSX profiling flags see SNPS3_HUD_RSX_USE_*.
///
/// @param		hTarget				Handle to target.
/// @param		uRSXFlags			RSX profiling flags.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set RSX profiling flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetRSXProfilingFlags(HTARGET hTarget, UINT64 uRSXFlags);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Gets RSX profiling flags.
///             See SNPS3_HUD_RSX_USE_*.
///
/// @param		hTarget				Handle to target.
/// @param		puRSXFlags			Pointer to UINT64 to hold RSX profiling flags.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved RSX profiling flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puRSXFlags was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetRSXProfilingFlags(HTARGET hTarget, UINT64 * puRSXFlags);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets VRAM capture flags.
///
/// @param		hTarget				Handle to target.
/// @param		uVRAMFlags			Value of VRAM capture flags. See SNPS3_ENABLE_VRAM_CAPTURE.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set VRAM capture flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetVRAMCaptureFlags(HTARGET hTarget, UINT64 uVRAMFlags);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Gets VRAM capture flags.
///
/// @param		hTarget				Handle to target.
/// @param		puVRAMFlags			Pointer to UINT64 to hold VRAM capture flags. See SNPS3_ENABLE_VRAM_CAPTURE.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.5.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.5.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved VRAM capture flags.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puVRAMFlags was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetVRAMCaptureFlags(HTARGET hTarget, UINT64 * puVRAMFlags);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Perform VRAM capture on process. Image is saved as bitmap.
///             Note that due to a hardware limitation, it is necessary to wait for
///             at least 500ms after loading a process before calling this function, otherwise
///             it will fail.
///
/// @param		hTarget				Handle to target.
/// @param		uPID				Process to capture.
/// @param		pVRAMInfo			Pointer to instance of SNPS3VRAMInfo containing parameters for capture 
///                             	(see SNPS3GetVRAMInformation to get parameters from target).
///                             	If NULL, values will automatically be retrieved from target.
///                             	Set the pVRAMInfo->uBPAddressBP member to zero, to not use a breakpoint.
/// @param		czpFileName			Pointer to char array containing filename to write capture to. UTF-8 null-terminated string.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully captured VRAM.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Either czpFileName was NULL pointer or display mode (color value) in pVRAMInfo incorrect.\n
///             SN_E_OUT_OF_MEM - Not enough memory to store VRAM capture.\n
///             SN_E_ERROR - Error reading VRAM data or saving capture file.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3VRAMCapture(HTARGET hTarget, UINT32 uPID, SNPS3VRAMInfo *pVRAMInfo, const char *czpFileName);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Enable VRAM capture for a target (sets VRAM capture flag to enabled and GCM debug flag on).
///
/// @param		hTarget				Handle to target.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 2.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 2.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 2.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully enabled VRAM capture.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3EnableVRAMCapture(HTARGET hTarget);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Clears the exception state of a thread and causes it to exit.
///
/// @param		hTarget				Handle to target.
/// @param		uPID				Process ID.
/// @param		uTID				Thread ID.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully cleared thread exception state.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3ThreadExceptionClean(HTARGET hTarget, UINT32 uPID, UINT64 uTID);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Returns information about the specified transfer ID.
///
/// @param		hTarget				Handle to target.
///	@param		uTXID				File transfer ID.
///	@param		pTransferList		Pointer to transfer structure to be populated.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.1.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.1.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved file transfer information.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetFileTransferInfo(HTARGET hTarget, const UINT32 uTXID, SNPS3Transfer *pTransferInfo);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Returns the list of files currently being transferred to the PS3. 
///             The list returned depends on the size of pTransferList. If the data stored on the TM is greater
///             than the size of pTransferList, pTransferList will store data for the most recently uploaded files.
///             Call this function with pTransferList = NULL to get the buffer size (*puCount * sizeof(SNPS3Transfer))
///             required to store the entire list.
///
/// @param		hTarget				Handle to target.
///
///	@param		puCount				[in] Maximum number of SNPS3Transfer instances buffer can hold.\n
///                             	[out] Total number of file entries currently stored on the TM. 
///	@param		pTransferList		Pointer to first entry in the transfer list.
///
///	@note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.1.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.1.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved file transfer list or size required for entire list.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puCount was NULL pointer.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetFileTransferList(HTARGET hTarget, UINT32 *puCount, SNPS3Transfer * pTransferList);

//////////////////////////////////////////////////////////////////////////////
///
///  @deprecated  Use SNPS3InstallGameEx().
///  @brief  Use SNPS3InstallGameEx().
///
//////////////////////////////////////////////////////////////////////////////
SNDEPRECATED("use SNPS3InstallGameEx instead")
SNAPI SNRESULT SNPS3InstallGame(HTARGET hTarget, const char *pszPath);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Install a game on the PS3.
///             Copies all the files located in the same directory
///             as param.sfo to a newly created game directory on the target. The new game directory is created 
///             in dev_hdd0/game, and is named according to the title specified in PARAM.SFO.
///             Only HDD boot games are supported. Error message 0x80028f14 will be returned 
///             (see SNPS3GetErrorQualifier()) if SNPS3InstallGameEx() is used to install game data (GD).
///
///             This function returns success as soon as the file transfers have been initiated.
///             Call SNPS3WaitForFileTransfer() with puTXID as a parameter if you want to wait for the install
///             to complete. Note that if there are no other files in the directory containing PARAM.SFO,
///             then puTXID will be zero on return and so a call to SNPS3WaitForFileTransfer() will return SN_E_BAD_PARAM.
///
/// @param		hTarget				Handle to the target.
///	@param		pszPath				Full path to desired param.sfo file on the host. UTF-8 null-terminated string.
/// @param		pszTitleId			The installed game title.
/// @param		pszTargetPath		The full path of the game installed on the target. UTF-8 null-terminated string.
/// @param		puTXID				Pointer to UINT32 to store last transfer ID.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.3.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.3.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.3.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully initiated game file transfers.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszPath was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.\n
///             SN_E_FILE_ERROR - Error reading param.sfo - please ensure that the full path has been supplied.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3InstallGameEx(HTARGET hTarget, const char *pszPath, char **pszTitleId, char **pszTargetPath, UINT32* puTXID);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Install a game package (pkg file) on the PS3.
///             Function returns when installation is 100% complete.
///             To check the path of the installed package, call SNPS3Kick() after this function has returned. 
///             A DBG target event SNPS3_DBG_EVENT_INSTALL_PACKAGE_PATH will be received.
///
/// @param		hTarget				Handle to the target.
///	@param		pszPath				Path to desired pkg file on the host. UTF-8 null-terminated string.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.6.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.6.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.6.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully installed package.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszPkgPath was NULL pointer.\n
///             SN_E_FILE_ERROR - pkg file does not exist or could not be opened\n
///             SN_E_COMMS_ERR - Error communicating with target.\n
///             SN_E_TIMEOUT - Command timed out.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3InstallPackage(HTARGET hTarget, const char *pszPkgPath);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Waits until a file transfer given by a transfer id has been processed.
///             Check pFTValue on return for status.
///             If the timeout value is 0, the function will return SN_E_TIMEOUT immediately and 
///             pFTValue will signal the file transfer status.
///             If this function is called and the file transfer is pending or in progress,
///             SNPS3Kick() is called internally - therefore if you have registered an FTP 
///             event handler via SNPS3RegisterFTPEventHandler(), then your callback may be 
///             called during execution of SNPS3WaitForFileTransfer().
///
/// @param		hTarget				Handle to the target.
/// @param		uTXID				The transfer ID.
/// @param		pFTValue			Pointer to the file transfer status. Can be NULL, except when a timeout of zero is specified.
/// @param		nMillisecondTimeout	Millisecond timeout value.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.3.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.3.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.3.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK	- Function executed successfully. Check the pFTValue parameter for more details of the file transfer status.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Zero timeout was specified and pFTValue was NULL.\n
///             SN_E_TM_NOT_RUNNING - Server was shutdown during function call.\n
///             SN_E_TIMEOUT - Timed out.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3WaitForFileTransfer(HTARGET hTarget, const UINT32 uTXID, TMAPI_FT_NOTIFY *pFTValue, const UINT32 nMillisecondTimeout);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Clear the file transfer list, based on status.
///
/// @param		hTarget				Handle to the target.
/// @param		filter				Transfer status to clear. See SNPS3Transfer(). 
///                                 Use 0xffffffff to clear all.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK	- Function executed successfully.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RemoveTransferItemsByStatus(HTARGET hTarget, UINT32 filter);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Uninstall a game from the target.
///
/// @param		hTarget				Handle to the target.
///	@param		pszTitleId			Title of game directory to be removed (as per corresponding param.sfo file).
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.3.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.3.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.3.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully uninstalled game.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszTitleId was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UninstallGame(HTARGET hTarget, const char *pszTitleId);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Uninstall a game from the target.
///
/// @param		hTarget				Handle to the target.
///	@param		pszTitleId			Title of game directory to be removed (as per corresponding param.sfo file).
/// @param		timeoutMs			Time before operation times out in milliseconds
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.3.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.3.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.3.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully uninstalled game.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszTitleId was NULL pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.\n
///				SN_E_TIMEOUT - Timeout value not sufficient for operation.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UninstallGameEx(HTARGET hTarget, const char *pszTitleId, UINT timeoutMs);

// @name <ChModFilePermissions>
//@{
//! ReadWrite permissions - used with SNPS3CHMod.
const UINT32 SNPS3_FILE_PERMISSION_RW_USR	= 384; 
//! ReadOnly permissions - used with SNPS3CHMod.
const UINT32 SNPS3_FILE_PERMISSION_R_USR	= 256;
//@}

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets the read/write/execute permissions for a file on the target.
///
/// @param		hTarget				Handle to the target.
///	@param		pszFilePath			Full file path. UTF-8 null-terminated string.
///	@param		uMode				See \link SNPS3_FILE_PERMISSION_RW_USR SNPS3_FILE_PERMISSION_RW_USR\endlink and 
///                             	\link SNPS3_FILE_PERMISSION_R_USR SNPS3_FILE_PERMISSION_R_USR\endlink.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set file permissions.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszFilePath was NULL pointer.\n
///             SN_E_NOT_SUPPORTED_IN_SDK_VERSION - Not supported by current SDK version. Please update the SDK.\n
///             SN_E_DATA_TOO_LONG - pszFilePath exceeded maximum path length for system (1024 for PS3).\n
///             SN_E_FILE_ERROR - Failed to set permissions on file.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3CHMod(HTARGET hTarget, const char *pszFilePath, UINT32 uMode);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets access and modified times for a file on the PS3.
///             Times are specified in Unix (time_t / UTC) time.
///
/// @param		hTarget				Handle to the target.
///	@param		pszFilePath			Full file path. UTF-8 null-terminated string.
///	@param		uActTimeT			New access time (time_t / UTC).
///	@param		uModTimeT			New modified time (time_t / UTC).
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set file times.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszFilePath was NULL pointer.\n
///             SN_E_NOT_SUPPORTED_IN_SDK_VERSION - Not supported by current SDK version. Please update the SDK.\n
///             SN_E_DATA_TOO_LONG - pszFilePath exceeded maximum path length for system (1024 for PS3).\n
///             SN_E_FILE_ERROR - Failed to set file time.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetFileTime(HTARGET hTarget, const char *pszFilePath, UINT64 uActTimeT, UINT64 uModTimeT);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Retrieve the available free size on a PS3 file system.
///
/// @param		hTarget				Handle to the target.
///	@param		fileSystemDir		File system directory. Supported value is "/dev_hdd0/"
///	@param		pBlockSize			Size of a block.
///	@param		pFreeBlockCount		Number of free block available.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully get free size.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszFilePath was invalid.\n
///             SN_E_DATA_TOO_LONG - pszFilePath exceeded maximum path length for system (1024 for PS3).\n
///             SN_E_FILE_ERROR - Failed to retrieve the available size.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3FSGetFreeSize(HTARGET hTarget, const char *fileSystemDir, UINT32* pBlockSize, UINT64* pFreeBlockCount);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Retrieves the current XMB settings.
///             If the supplied buffer is not large enough,
///             settings will be truncated. To get required size for all XMB settings, set pSettings to 
///             NULL and call the function. On return, puSize will then contain the required size.
///
/// @param		hTarget				Handle to the target.
///	@param		pSettings			String to contain XMB settings. See table below.
///	@param		puSize				Size of pSettings. If pSettings is NULL, this will be set to the required size.
///	@param		bUpdateCache		If set to TRUE, updates internal cache from target, otherwise the last saved XMB settings will be used.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully retrieved XMB settings.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - puSize was NULL pointer.
///             SN_E_COMMS_ERR - Error communicating with target.
///
///	@details	pSettings will contain a zero-terminated string that will be in the following format: key1=value1,key2=value2, and so on. Possible values are:\n\n
///             <table>
///             	<tr>
///             		<th>Key</th>
///             		<th>Setting in XMB</th>
///             		<th>Possible Values</th>
///             	</tr>
///             	<tr>
///             		<td>version</td>
///             		<td>N/A</td>
///             		<td>1 (current version)</td>
///             	</tr>
///             	<tr>
///             		<td>System/fakeFreeSpace</td>
///             		<td>[Debug Settings] >\n[Fake Free Space]</td>
///             		<td>0: Off, 1: On, 2: On (Ignored in NP Trophy)</td>
///             	</tr>
///             	<tr>
///             		<td>System/fakeLimitSize</td>
///             		<td>[Debug Settings] >\n[Fake Limit Size]</td>
///             		<td>A positive integer value</td>
///             	</tr>
///             	<tr>
///             		<td>System/updateServerUrl</td>
///             		<td>[Debug Settings] >\n[Update Server URL]</td>
///             		<td>A string value</td>
///             	</tr>
///             	<tr>
///             		<td>System/wolDex</td>
///             		<td>[Debug Settings] >\n[Wake On LAN]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System/dispHddSpace</td>
///             		<td>[Debug Settings] >\n[Display HDD Free Space]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System/powerOnReset</td>
///             		<td>[Debug Settings] >\n[PowerOnReset]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System/nickname</td>
///             		<td>[System Settings] >\n[System Name]</td>
///             		<td>A string value (up to 31 characters including NULL character)</td>
///             	</tr>
///             	<tr>
///             		<td>System/fakeSavedataOwner</td>
///             		<td>[Debug Settings] >\n[Fake Save Data Owner]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System/matEnable</td>
///             		<td>[Debug Settings] >\n[Memory Access Trap]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System/debugGameType</td>
///             		<td>[Debug Settings] >\n[Game Type (Debugger)]</td>
///             		<td>0: Disc Boot Game, 1: HDD Boot Game, 2: Patch, 3: PARAM.SFO</td>
///             	</tr>
///             	<tr>
///             		<td>System/debugBootPath</td>
///             		<td>[Debug Settings] >\n[GameContentUtil Boot Path (Debugger)]</td>
///             		<td>0: For Development, 1: For Release</td>
///             	</tr>
///             	<tr>
///             		<td>System/debugDirName</td>
///             		<td>[Debug Settings] >\n[GameContentUtil Debug Path (Debugger)]</td>
///             		<td>A string value (up to 31 characters including NULL character, alpha-numeric characters only)</td>
///             	</tr>
///             	<tr>
///             		<td>System/appHomeBootPath</td>
///             		<td>[Debug Settings] >\n[GameContentUtil Boot Path (/app_home/PS3_GAME/)]</td>
///             		<td>0: For Development, 1: For Release</td>
///             	</tr>
///             	<tr>
///             		<td>System/disable15Timeout</td>
///             		<td>[Debug Settings] >\n[Disable ExitGame Timeout]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System/buttonAssign</td>
///             		<td>[Debug Settings] >\n[O Button Behavior]</td>
///             		<td>0: Enter, 1: Back</td>
///             	</tr>
///             	<tr>
///             		<td>System.tmp/fakeOtherRegion</td>
///             		<td>[Debug Settings] >\n[Fake Other Region]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>System.tmp/dispMsgDialogUtilErrorcode</td>
///             		<td>[Debug Settings] >\n[MsgDialogUtil Displace Errorcode]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>Display/gameResolution</td>
///             		<td>[Debug Settings] >\n[Game Output Resolution (Debugger)]</td>
///             		<td>0: 480 (4:3)<br />1: 480 (16:9)<br />2: 576 (4:3)<br />3: 576 (16:9)<br />4: 720<br />5: 960 x 1080<br />6: 1280 x 1080<br />7: 1440 x 1080<br />8: 1600 x 1080<br />9: 1920 x 1080</td>
///             	</tr>
///             	<tr>
///             		<td>Display/hdcp</td>
///             		<td>[Debug Settings] >\n[HDCP]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>Sound/gameSound</td>
///             		<td>[Debug Settings] >\n[Game Output Sound (Debugger)]</td>
///             		<td>0: Maximum Number of Channels Set on [Sound Settings] >\n[Audio Output Settings]<br />1: 2 ch<br />2: 2 ch (Downmix: 5.1 ch -> 2 ch)<br />3: 2 ch (Downmix: 7.1 ch -> 2 ch)<br />4: 5.1 ch<br />5: 5.1 ch (Downmix: 7.1 ch -> 5.1 ch)<br />6: 7.1 ch<br />7: Dolby Digital<br />8: Dolby Digital (Downmix: 7.1 ch -> 5.1 ch)<br />9: DTS<br />10: DTS (Downmix: 7.1 ch -> 5.1 ch)</td>
///             	</tr>
///             	<tr>
///             		<td>Network/device</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nSelect a connection method</td>
///             		<td>0: Wired</td>
///             	</tr>
///             	<tr>
///             		<td>Network/etherMode</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nSpeed and Duplex</td>
///             		<td>0: Auto-Detect<br />1: 10BASE-T Half-Duplex<br />2: 10BASE-T Full-Duplex<br />3: 100BASE-TX Half-Duplex<br />4: 100BASE-TX Full-Duplex<br />5: 1000BASE-T Half-Duplex<br />6: 1000BASE-T Full-Duplex</td>
///             	</tr>
///             	<tr>
///             		<td>Network/howToSetupIp</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nIP Address Setting</td>
///             		<td>0: Automatic, 1: Manual, 2: PPPoE</td>
///             	</tr>
///             	<tr>
///             		<td>Network/dhcpHostName</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nDHCP Host Name</td>
///             		<td>A string value (up to 255 characters including the NULL character)</td>
///             	</tr>
///             	<tr>
///             		<td>Network/authName</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nPPPoE User Name</td>
///             		<td>A string value (up to 127 characters including the NULL character)</td>
///             	</tr>
///             	<tr>
///             		<td>Network/authKey</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nPPPoE Password</td>
///             		<td>A string value (up to 127 characters including the NULL character)</td>
///             	</tr>
///             	<tr>
///             		<td>Network/ipAddress</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nIP Address</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Network/netmask</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nSubnet Mask</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Network/defaultRoute</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nDefault Router</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Network/dnsFlag</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nDNS Setting</td>
///             		<td>0: Automatic, 1: Manual</td>
///             	</tr>
///             	<tr>
///             		<td>Network/primaryDns</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nPrimary DNS</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Network/secondaryDns</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nSecondary DNS</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Network/httpProxyFlag</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nProxy Server</td>
///             		<td>0: Do Not Use, 1: Use</td>
///             	</tr>
///             	<tr>
///             		<td>Network/httpProxyServer</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nProxy Server > Address</td>
///             		<td>A string value (up to 255 characters including the NULL character)</td>
///             	</tr>
///             	<tr>
///             		<td>Network/httpProxyPort</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nProxy Server > Port Number</td>
///             		<td>A numerical value from 0-65535</td>
///             	</tr>
///             	<tr>
///             		<td>Network/mtu</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nMTU</td>
///             		<td>A numerical value from 576-1500</td>
///             	</tr>
///             	<tr>
///             		<td>Network/upnpFlag</td>
///             		<td>[Network Settings] >\n[Internet Connection Settings] >\nUPnP</td>
///             		<td>0: Disable, 1: Enable</td>
///             	</tr>
///             	<tr>
///             		<td>Network/emulationType</td>
///             		<td>[Debug Settings] >\n[Network Emulation Settings]</td>
///             		<td>0: Off, 1: Option 1, 2: Options 2, 3: Option 3</td>
///             	</tr>
///             	<tr>
///             		<td>Network/adhocSsidPrefix</td>
///             		<td>[Debug Settings] >\n[Adhoc SSID Prefix]</td>
///             		<td>A string value consisting of a 3-character prefix</td>
///             	</tr>
///             	<tr>
///             		<td>Network.eth2/howToSetupIp</td>
///             		<td>[Debug Settings] >\n[Connection Settings for Debug (Dual Settings)]</td>
///             		<td>0: Automatic, 1: Manual</td>
///             	</tr>
///             	<tr>
///             		<td>Network.eth2/dhcpHostName</td>
///             		<td>[Debug Settings] >\n[Connection Settings for Debug (Dual Settings)] >\nDHCP Host Name</td>
///             		<td>A string value (up to 255 characters including the NULL character)</td>
///             	</tr>
///             	<tr>
///             		<td>Network.eth2/ipAddress</td>
///             		<td>[Debug Settings] >\n[Connection Settings for Debug (Dual Settings)] >\nIP Address</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Network.eth2/netmask</td>
///             		<td>[Debug Settings] >\n[Connection Settings for Debug (Dual Settings)] >\nSubnet Mask</td>
///             		<td>A string value (up to 17 characters including the NULL character) of the format XXX.XXX.XXX.XXX where XXX is a numerical value from 0-255</td>
///             	</tr>
///             	<tr>
///             		<td>Music/gameBgmPlayback</td>
///             		<td>[Debug Settings] >\n[BGM Player (Debugger)]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>Music/dummyBgmPlayer</td>
///             		<td>[Debug Settings] >\n[Dummy BGM Player Debug]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>np/debug</td>
///             		<td>[Debug Settings] >\n[NP Debug]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>np/debugDrmError</td>
///             		<td>[Debug Settings] >\n[NPDRM Debug]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>np/debugDrmClock</td>
///             		<td>[Debug Settings] >\n[NPDRM Clock Debug]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>np/titleId</td>
///             		<td>[Debug Settings] >\n[Service ID]</td>
///             		<td>A string value (up to 36 characters)</td>
///             	</tr>
///             	<tr>
///             		<td>np/env</td>
///             		<td>[Debug Settings] >\n[NP Environment]</td>
///             		<td>A string value (up to 8 characters)</td>
///             	</tr>
///             	<tr>
///             		<td>np/npAdClockDiff</td>
///             		<td>[Debug Settings] >\n[PlayStation&reg;Store Ad Clock]</td>
///             		<td>0: Off, values >0: the time difference</td>
///             	</tr>
///             	<tr>
///             		<td>np/gameUpdateImposeTest</td>
///             		<td>[Debug Settings] >\n[Game Update Impose Test]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>np/dummyInGameXMB</td>
///             		<td>[Debug Settings] >\n[Dummy XMB&trade; (in game) Debug]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>Device/ieee802.11</td>
///             		<td>[Debug Settings] >\n[WLAN Device]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>Upload/debug</td>
///             		<td>[Debug Settings] >\n[Video UploadDebug]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             	<tr>
///             		<td>libad/adServerUrl</td>
///             		<td>[Debug Settings] >\n[MediatedServices: Mediator URL]</td>
///             		<td>A string value (up to 1023 characters)</td>
///             	</tr>
///             	<tr>
///             		<td>libad/adCatalogVersion</td>
///             		<td>[Debug Settings] >\n[MediatedServices: Provider Data]</td>
///             		<td>A string value (up to 1023 characters)</td>
///             	</tr>
///             	<tr>
///             		<td>libad/adEnableNotification</td>
///             		<td>[Debug Settings] >\n[MediatedServices: Notifications]</td>
///             		<td>0: Off, 1: On</td>
///             	</tr>
///             </table>
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetXMBSettings(HTARGET hTarget, char *pSettings, UINT *puSize, BOOL bUpdateCache);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets the target's current XMB settings.
///             Note: This function uses the windows temporary directory. The Windows function GetTempPath() is used 
///             to retrieve the path to the windows temp directory. If case-sensitive file serving is turned on 
///             (via SNPS3SetCaseSensitiveFileServing()) and the string returned by GetTempPath() does not case-sensitive-match 
///             the actual folder in windows, then SNPS3SetXMBSettings() will fail.
///             GetTempPath() first checks the TMP system environment variable for the temp path. 
///             An example of the failure described above is if TMP has the value C:\\Windows\\TEMP and your actual folder is C:\\Windows\\Temp.
///
/// @param		hTarget				Handle to target.
///	@param		pSettings			String containing the new XMB settings. 
///                             	The settings should be in the following format: key1=value1,key2=value2, and so on.
///                                 For a list of possible values, see SNPS3GetXMBSettings().
///  @param		bResetAfter			Indicates whether or not to reset target after applying settings.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set XMB settings.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - Failed to set XMB settings.
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetXMBSettings(HTARGET hTarget, const char *pSettings, BOOL bResetAfter);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Enables/disables XMB settings for target.
///             Note that SNPS3SetXMBSettings() automatically enables XMB settings, so it overrides the
///             value set by SNPS3EnableXMBSettings().
///
/// @param		hTarget				Handle to target.
/// @param		bEnabled			Non-zero to enable, zero to disable.
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully enabled/disabled XMB settings.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///             SN_E_FILE_ERR - Error deleting XMB settings file (in the case of disabling XMB settings).
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3EnableXMBSettings(HTARGET hTarget, BOOL bEnable);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets the target's custom PARAM.SFO path.
///             To use this PARAM.SFO when loading a process via SNPS3ProcessLoad(), the debug flags parameter 
///             must be set appropriately.
///             Set the SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USECUSTOMDIR flag and clear the 
///             SNPS3_PROCESS_LOAD_FLAG_PARAMSFO_USEELFDIR flag.
///
/// @param		hTarget				Handle to target.
///	@param		paramSfoPath		Custom PARAM.SFO directory path. UTF-8 null-terminated string.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set custom PARAM.SFO path.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - paramSfoPath was null pointer.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetCustomParamSFOMappingDirectory(HTARGET hTarget, const char *pParamSfoPath);


/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Formats the target's HDD.
///
/// @param		hTarget				Handle to target.
/// @param		initRegistry		Set it to 1 if you also want to clear the registry 
///
/// @note	Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully formatted HDD.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TARGET_IS_POWERED_OFF - Target is powered off. Power on the target first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3FormatHDD(HTARGET hTarget, UINT32 initRegistry);


/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Sets the TM server logging category.
///
/// @param		categories		    Logging category.
///                                (See \link ApiBase::TMAPI_LOG_CATEGORY TMAPI_LOG_CATEGORY\endlink).
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully get the server logging category.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3SetLogOptions(TMAPI_LOG_CATEGORY categories);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Gets the TM server logging category.
///
/// @param		categories		    Logging category.
///                                (See \link ApiBase::TMAPI_LOG_CATEGORY TMAPI_LOG_CATEGORY\endlink).
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set the server logging category.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3GetLogOptions(TMAPI_LOG_CATEGORY* categories);

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Get test parameters via TSMP
///
/// @param		hTarget				Handle to target
/// @param		pResult				Byte to write result to (standard TSMP error codes)
/// @param		paData				Array of data (will be sent to target and will hold received data)
///
///  @return	\link ApiBase::SNRESULT SNRESULT:\endlink\n
///				SN_E_BAD_PARAM - puResult and/or paData were NULL parameters

SNAPI SNRESULT SNPS3ProcessGetTestParam(HTARGET hTarget, BYTE *pResult, BYTE* paData);

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Set test parameters via TSMP
///
/// @param		hTarget				Handle to target
/// @param		pResult				Byte to write result to (standard TSMP error codes)
/// @param		paData				Array of data
///
///  @return	\link ApiBase::SNRESULT SNRESULT:\endlink\n
///				SN_E_BAD_PARAM - puResult and/or paData were NULL parameters

SNAPI SNRESULT SNPS3ProcessSetTestParam(HTARGET hTarget, BYTE *pResult, BYTE* paData);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Enables/disables the internal kick flag.
///             Set this flag to allow callbacks to be processed during calls to 
///             PS3 API function calls. This flag is off by default, in which case
//				SNPS3Kick() must be called in order for user callbacks to be invoked.
///
/// @param		bEnable		     Non-zero to enable, zero to disable.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully set the internal kick flag.\n
///             SN_S_NO_ACTION - Internal kick already enabled or disabled.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3EnableInternalKick(BOOL bEnable);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Registers an idle worker callback.
///
/// @param		pData	A pointer to user data that is passed to the idle worker callback.
/// @param		pfnIdleWorkerCallback	Pointer to callback.
///
/// @details	An idle worker callback can be registered for applications that require work to
///				to be performed during PS3API function calls. This is useful for GUI applications that
///				require their messages to be pumped to prevent an unresponsive GUI during time consuming PS3API function calls.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully registered the idle worker callback.\n
///				SN_S_REPLACED - Callback function already registered and has been replaced.\n
///             SN_E_BAD_PARAM - Invalid pointer to idle worker callback function.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3RegisterIdleWorker(TMAPI_IdleWorker pfnIdleWorkerCallback, void* pData);

//////////////////////////////////////////////////////////////////////////////
///
///  @brief     Unregister the current idle worker callback.
///
///  @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully unregistered the idle worker callback.\n
///             SN_S_NO_ACTION - No idle worker callback function registered.\n
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3UnRegisterIdleWorker();

//////////////////////////////////////////////////////////////////////////////
/// @enum		SNPS3_FILE_TRACE_TYPE
///	@brief		Type of file trace data to be used in conjunction with SNPS3_FILE_TRACE_LOG_TYPE_*.
//////////////////////////////////////////////////////////////////////////////

enum SNPS3_FILE_TRACE_TYPE {

	// @name <Type1FileTraceData>
	//@{
	//! Type 1.
	SNPS3_FT_GET_BLOCK_SIZE = 1,
	SNPS3_FT_STAT,
	SNPS3_FT_WIDGET_STAT,
	SNPS3_FT_UNLINK,
	SNPS3_FT_WIDGET_UNLINK,
	SNPS3_FT_RMDIR,
	SNPS3_FT_WIDGET_RMDIR,
	//@}

	// @name <Type2FileTraceData>
	//@{
	//! Type 2.
	SNPS3_FT_RENAME = 14,
	SNPS3_FT_WIDGET_RENAME,
	//@}

	// @name <Type3FileTraceData>
	//@{
	//! Type 3.
	SNPS3_FT_TRUNCATE = 18,
	SNPS3_FT_TRUNCATE_NO_ALLOC,
	SNPS3_FT_TRUNCATE2,
	SNPS3_FT_TRUNCATE2_NO_INIT,
	//@}

	// @name <Type4FileTraceData>
	//@{
	//! Type 4.
	SNPS3_FT_OPENDIR = 24,
	SNPS3_FT_WIDGET_OPENDIR,
	SNPS3_FT_CHMOD,
	SNPS3_FT_MKDIR,
	//@}

	// @name <Type6FileTraceData>
	//@{
	//! Type 6.
	SNPS3_FT_UTIME = 29,
	//@}

	// @name <Type8FileTraceData>
	//@{
	//! Type 8.
	SNPS3_FT_OPEN = 33,
	SNPS3_FT_WIDGET_OPEN,
	//@}

	// @name <Type9FileTraceData>
	//@{
	//! Type 9.
	SNPS3_FT_CLOSE = 35,
	SNPS3_FT_CLOSEDIR,
	SNPS3_FT_FSYNC,
	SNPS3_FT_READDIR,
	SNPS3_FT_FSTAT,
	SNPS3_FT_FGET_BLOCK_SIZE,
	//@}

	// @name <Type10FileTraceData>
	//@{
	//! Type 10.
	SNPS3_FT_READ = 47,
	SNPS3_FT_WRITE,
	SNPS3_FT_GET_DIR_ENTRIES,
	//@}

	// @name <Type11FileTraceData>
	//@{
	//! Type 11.
	SNPS3_FT_READ_OFFSET = 50,
	SNPS3_FT_WRITE_OFFSET,
	//@}

	// @name <Type12FileTraceData>
	//@{
	//! Type 12.
	SNPS3_FT_FTRUNCATE = 52,
	SNPS3_FT_FTRUNCATE_NO_ALLOC,
	//@}

	// @name <Type13FileTraceData>
	//@{
	//! Type 13.
	SNPS3_FT_LSEEK = 56,
	//@}

	// @name <Type14FileTraceData>
	//@{
	//! Type 14.
	SNPS3_FT_SET_IO_BUFFER = 57,
	//@}

	//! Special marker
	SNPS3_FT_OFFLINE_END = 9999,
};

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG
/// @brief		Contains file trace notification data.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG
{
	UINT64 ulSerialID;
	UINT32 ulAPIType;
	UINT32 ulStatus;
	UINT32 ulProcessID;
	UINT32 ulThreadID;
	UINT64 ulTimeBaseStartOfTrace;
	UINT64 ulTimeBase;
	UINT32 ulBackTraceLength;
	//  BYTE arrBackTrace[];
	//	BYTE arrData[];
};

// @name <FileTraceNotificationStatuses>
//@{
//! File trace notification status value.
const UINT32 SNPS3_FILE_TRACE_STATUS_PROCESSED	= 0; 
const UINT32 SNPS3_FILE_TRACE_STATUS_RECEIVED	= 1;
const UINT32 SNPS3_FILE_TRACE_STATUS_WAITING	= 2; 
const UINT32 SNPS3_FILE_TRACE_STATUS_PROCESSING	= 3; 
const UINT32 SNPS3_FILE_TRACE_STATUS_SUSPENDED	= 4; 
const UINT32 SNPS3_FILE_TRACE_STATUS_FINISHED	= 5;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_1
/// @brief		Contains file trace notification data for type 1 log entries.
///				If ulPathLength is greater than zero, path is a UTF-8 null-terminated string.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_1
{
	UINT32 ulPathLength;
	//  char szPath[];
};

// @name <FileTraceLogsType1>
//@{
//! Type 1 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_GET_BLOCK_SIZE;
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_STAT;
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_WIDGET_STAT;
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_UNLINK;
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_WIDGET_UNLINK;
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_RMDIR;
typedef SNPS3_FILE_TRACE_LOG_TYPE_1 SNPS3_FT_LOG_WIDGET_RMDIR;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_2
/// @brief		Contains file trace notification data for type 2 log entries.
///				If a given path length is greater than zero, path is a UTF-8 null-terminated string.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_2
{
	UINT32 ulPathLength1;
	UINT32 ulPathLength2;
	//  char szPath1[];
	//  char szPath2[];
};

// @name <FileTraceLogsType2>
//@{
//! Type 2 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_2 SNPS3_FT_LOG_RENAME;
typedef SNPS3_FILE_TRACE_LOG_TYPE_2 SNPS3_FT_LOG_WIDGET_RENAME;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_3
/// @brief		Contains file trace notification data for type 3 log entries.
///				If ulPathLength is greater than zero, path is a UTF-8 null-terminated string.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_3
{
	UINT64 ulArg;
	UINT32 ulPathLength;
	//  char szPath[];
};

// @name <FileTraceLogsType3>
//@{
//! Type 3 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_3 SNPS3_FT_LOG_TRUNCATE;
typedef SNPS3_FILE_TRACE_LOG_TYPE_3 SNPS3_FT_LOG_TRUNCATE_NO_ALLOC;
typedef SNPS3_FILE_TRACE_LOG_TYPE_3 SNPS3_FT_LOG_TRUNCATE2;
typedef SNPS3_FILE_TRACE_LOG_TYPE_3 SNPS3_FT_LOG_TRUNCATE2_NO_INIT;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_4
/// @brief		Contains file trace notification data for type 4 log entries.
///				If ulPathLength is greater than zero, path is a UTF-8 null-terminated string.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_4
{
	UINT32 ulMode;
	UINT32 ulPathLength;
	//  char szPath[];
};

// @name <FileTraceLogsType4>
//@{
//! Type 4 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_4 SNPS3_FT_LOG_OPENDIR;
typedef SNPS3_FILE_TRACE_LOG_TYPE_4 SNPS3_FT_LOG_WIDGET_OPENDIR;
typedef SNPS3_FILE_TRACE_LOG_TYPE_4 SNPS3_FT_LOG_CHMOD;
typedef SNPS3_FILE_TRACE_LOG_TYPE_4 SNPS3_FT_LOG_MKDIR;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_6
/// @brief		Contains file trace notification data for type 6 log entries.
///				If ulPathLength is greater than zero, path is a UTF-8 null-terminated string.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_6
{
	UINT64 ulArg1;
	UINT64 ulArg2;
	UINT32 ulPathLength;
	//  char szPath[];
};

// @name <FileTraceLogsType6>
//@{
//! Type 6 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_6 SNPS3_FT_LOG_UTIME;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FT_STAT
///	@brief		Contains file stat notification data.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FT_STAT
{
	UINT32 st_mode;
	INT32 st_uid;
	INT32 st_gid;
	__time64_t st_atime;
	__time64_t st_mtime;
	__time64_t st_ctime;
	UINT64 st_size;
	UINT64 st_block_size;
};

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FT_PROCESS_INFO
///	@brief		Contains file process notification data.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FT_PROCESS_INFO
{
	UINT64 ulVFSID;
	UINT64 ulFD;
};

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_8
/// @brief		Contains file trace notification data for type 8 log entries.
///				If ulPathLength is greater than zero, path is a UTF-8 null-terminated string.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_8
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
	UINT32 ulArg1;
	UINT32 ulArg2;
	UINT32 ulArg3;
	UINT32 ulArg4;
	UINT32 ulVArgLength;
	UINT32 ulPathLength;
	//  BYTE arrVArg[];
	//	char szPath[];
};

// @name <FileTraceLogsType8>
//@{
//! Type 8 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_8 SNPS3_FT_LOG_OPEN;
typedef SNPS3_FILE_TRACE_LOG_TYPE_8 SNPS3_FT_LOG_WIDGET_OPEN;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_9
/// @brief		Contains file trace notification data for type 9 log entries.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_9
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
};

// @name <FileTraceLogsType9>
//@{
//! Type 9 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_9 SNPS3_FT_LOG_CLOSE;
typedef SNPS3_FILE_TRACE_LOG_TYPE_9 SNPS3_FT_LOG_CLOSEDIR;
typedef SNPS3_FILE_TRACE_LOG_TYPE_9 SNPS3_FT_LOG_FSYNC;
typedef SNPS3_FILE_TRACE_LOG_TYPE_9 SNPS3_FT_LOG_READDIR;
typedef SNPS3_FILE_TRACE_LOG_TYPE_9 SNPS3_FT_LOG_FSTAT;
typedef SNPS3_FILE_TRACE_LOG_TYPE_9 SNPS3_FT_LOG_FGET_BLOCK_SIZE;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_10
/// @brief		Contains file trace notification data for type 10 log entries.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_10
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
	UINT32 ulSize;
	UINT64 ulAddress;
	UINT32 ulTxSize;
};

// @name <FileTraceLogsType10>
//@{
//! Type 10 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_10 SNPS3_FT_LOG_READ;
typedef SNPS3_FILE_TRACE_LOG_TYPE_10 SNPS3_FT_LOG_WRITE;
typedef SNPS3_FILE_TRACE_LOG_TYPE_10 SNPS3_FT_LOG_GET_DIR_ENTRIES;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_11
/// @brief		Contains file trace notification data for type 11 log entries.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_11
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
	UINT32 ulSize;
	UINT64 ulAddress;
	UINT64 ulOffset;
	UINT32 ulTxSize;
};

// @name <FileTraceLogsType11>
//@{
//! Type 11 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_11 SNPS3_FT_LOG_READ_OFFSET;
typedef SNPS3_FILE_TRACE_LOG_TYPE_11 SNPS3_FT_LOG_WRITE_OFFSET;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_12
/// @brief		Contains file trace notification data for type 12 log entries.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_12
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
	UINT64 ulTargetSize;
};

// @name <FileTraceLogsType12>
//@{
//! Type 12 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_12 SNPS3_FT_LOG_FTRUNCATE;
typedef SNPS3_FILE_TRACE_LOG_TYPE_12 SNPS3_FT_LOG_FTRUNCATE_NO_ALLOC;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_13
/// @brief		Contains file trace notification data for type 13 log entries.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_13
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
	UINT32 ulSize;
	UINT64 ulOffset;
	UINT64 ulCurPos;
};

// @name <FileTraceLogsType13>
//@{
//! Type 13 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_13 SNPS3_FT_LOG_LSEEK;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_FILE_TRACE_LOG_TYPE_14
/// @brief		Contains file trace notification data for type 14 log entries.
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_FILE_TRACE_LOG_TYPE_14
{
	SNPS3_FT_PROCESS_INFO ProcessInfo;
	UINT32 ulMaxSize;
	UINT32 ulPage;
	UINT32 ulContainerID;
};

// @name <FileTraceLogsType14>
//@{
//! Type 14 file trace log type.
typedef SNPS3_FILE_TRACE_LOG_TYPE_14 SNPS3_FT_LOG_SET_IO_BUFFER;
//@}

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Transfer an image file to the target's BD emulator.
///
/// @param		hTarget					Handle to target.
/// @param		pszSourceFileName		Disk image path. UTF-8 null-terminated string.
/// @param		pszDestinationDevice	Destination BD emulator device  ("/dev_bdemu/0" .. "/dev_bdemu/3")
/// @param		puTransferId    Pointer to a variable which receives the transfer id of the image (see SNPS3WaitForFileTransfer)
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 4.1.0 - latest version.\n
///				Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 4.1.0 - latest version.\n
///				Reference tool (DECR-1400J / DECR-1400A)						: SDK 4.1.0 - latest version.
///         
/// @details Monitor FTP,and TARGET events (see SNPS3RegisterFTPEventHandler(), SNPS3RegisterTargetEventHandler and SN_TGT_BD_ISOTRANSFER_FINISHED for more information).
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully formatted HDD.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TARGET_IS_POWERED_OFF - Target is powered off. Power on the target first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///				SN_E_NOT_SUPPORTED_IN_SDK_VERSION - The system software does not support the operation. (Upgrade may be necessary)
///             SN_E_BAD_PARAM - pszSourceFileName, or pszDestinationDevice is NULL or invalid\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3BDTransferImage(HTARGET hTarget, const char* pszSourceFileName, const char* pszDestinationDevice, UINT32* puTransferId);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Inserts the specified disc image into the BD emulator 
///
/// @param		hTarget			Handle to target.
/// @param		pszDeviceName	BD emulator device ("/dev_bdemu/0" .. "/dev_bdemu/3")
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 4.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 4.1.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 4.1.0 - latest version.
///
/// @details	It is possible to monitor TARGET events to detect changes made by other clients using SN_TGT_BD_MOUNTED.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully formatted HDD.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TARGET_IS_POWERED_OFF - Target is powered off. Power on the target first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///				SN_E_NOT_SUPPORTED_IN_SDK_VERSION - The system software does not support the operation. (Upgrade may be necessary)
///             SN_E_BAD_PARAM - pszDeviceName is NULL or invalid\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3BDInsert(HTARGET hTarget, const char* pszDeviceName);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Ejects the current disc image 
///
/// @param		hTarget			Handle to target.
/// @param		pszDeviceName	BD emulator device ("/dev_bdemu")
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 4.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 4.1.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 4.1.0 - latest version.
///
/// @details	It is possible to monitor TARGET events to detect changes made by other clients using SN_TGT_BD_UNMOUNTED.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully formatted HDD.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TARGET_IS_POWERED_OFF - Target is powered off. Power on the target first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///             SN_E_BAD_PARAM - pszDeviceName is NULL or invalid\n
///				SN_E_NOT_SUPPORTED_IN_SDK_VERSION - The system software does not support the operation. (Upgrade may be necessary)
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////


SNAPI SNRESULT SNPS3BDEject(HTARGET hTarget, const char* pszDeviceName);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Formats the BD emulator's storage device 
///
/// @param		hTarget			Handle to target.
/// @param		uFormatMode		Format mode (SNPS3_BD_FORMAT_TYPE_NORMAL or SNPS3_BD_FORMAT_QUICK)
/// @param		pszDeviceName	BD emulator device ("/dev_bdemu")
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 4.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 4.1.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 4.1.0 - latest version.
///
/// @details	Monitor TARGET events (see SNPS3RegisterTargetEventHandler and SN_TGT_BD_FORMAT_FINISHED for more information).
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successfully formatted HDD.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TARGET_IS_POWERED_OFF - Target is powered off. Power on the target first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///				SN_E_NOT_SUPPORTED_IN_SDK_VERSION - The system software does not support the operation. (Upgrade may be necessary)
///             SN_E_BAD_PARAM - pszDeviceName is NULL or invalid\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3BDFormat(HTARGET hTarget, const char* pszDeviceName, UINT32 uFormatMode);

// @name <BDFormatMode>
//@{
const UINT32 SNPS3_BD_FORMAT_TYPE_NORMAL = 0;
const UINT32 SNPS3_BD_FORMAT_TYPE_QUICK = 1;
//@}

//////////////////////////////////////////////////////////////////////////////
/// @struct		SNPS3_BD_QUERY_DATA
/// @brief		Contains information of a BD emulator device
//////////////////////////////////////////////////////////////////////////////

struct SNPS3_BD_QUERY_DATA
{
	UINT32 bdemu_data_size;
	/// 4
	UCHAR bdemu_total_entry;
	/// [0-3] Index of Mounted device
	UCHAR bdemu_selected_index;
	/// [0-3] Index of the Device the data belongs to
	UCHAR image_index;
	/// 0:INVAL, 1:BDSL, 2:BDDL, 3:DVDSL, 4:DVDDL
	UCHAR image_type;
	/// Null terminated UTF-8 string
	char image_file_name[128];			
	UINT64 image_file_size;
	/// Null terminated string
	char image_product_code[32];
	/// Null terminated string
	char image_producer[32];
	/// Null terminated string
	char image_author[32];
	/// YYYYMMDD (Not null terminated!)
	char image_date[8];
	/// Layer 0 information
	UINT32 image_sector_layer0;
	/// Layer 1 information (0 for XXXSL discs)
	UINT32 image_sector_layer1;
	/// Null terminated string
	char image_memorandum[32];
};

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Get information from the  BD emulator 
///
/// @param		hTarget			Handle to target.
/// @param		pszDeviceName	BD emulator device (/dev_bdemu/0 etc...)
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 4.1.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 4.1.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 4.1.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Successful query.\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_TARGET_IS_POWERED_OFF - Target is powered off. Power on the target first.\n
///             SN_E_BAD_TARGET - hTarget was invalid.\n
///				SN_E_NOT_SUPPORTED_IN_SDK_VERSION - The system software does not support the operation. (Upgrade may be necessary)
///             SN_E_BAD_PARAM - pszDeviceName is NULL or invalid\n
///             SN_E_COMMS_ERR - Error communicating with target.
///
//////////////////////////////////////////////////////////////////////////////

SNAPI SNRESULT SNPS3BDQuery(HTARGET hTarget, const char* pszDeviceName, SNPS3_BD_QUERY_DATA * pData);

#pragma pack(pop)
#pragma warning(default : 4200) 	// nonstandard extension used : zero-sized array in struct/union

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Search for targets in the specified IP range 
///
/// @param		szIpAddressFrom	Start of the IP address range
/// @param		szIpAddressTo	End of IP address range
/// @param		pfnCallback		Pointer to a callback function which receives search results. Callback has to be thread safe. End of search is indicated by passing NULL in szName.
/// @param		pUserData		Any user specified data to be used in the callback function
/// @param		nPort			Port to scan, automatic (-1) is recommended
///
/// @details	Ip address range should be reasonable, (e.g. not like 0.0.0.1 - 255.255.255.254), otherwise the search will take indefinite amount of time. 
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Search started successfully\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///             SN_E_BAD_PARAM - szIpAddressFrom or szIpAddressTo is NULL or invalid\n
///             SN_E_BUSY - Search is already in progress.\n
///
//////////////////////////////////////////////////////////////////////////////
SNAPI 	SNRESULT SNPS3SearchForTargets(const char* szIpAddressFrom, const char* szIpAddressTo, TMAPI_SearchTargetsCallback pfnCallback, void* pUserData, int nPort);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Stop searching for targets.
///				Calling this function during a search causes NULL to be passed as the szName parameter to the user callback 
///				(see \link TMAPI_SearchTargetsCallback TMAPI_SearchTargetsCallback \endlink), signifying that the search has ended.
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///
//////////////////////////////////////////////////////////////////////////////
SNAPI 	SNRESULT SNPS3StopSearchForTargets();

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Returns SN_E_BUSY if a search is already in progress 
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.7.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.7.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.7.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - No search is in progress\n
///             SN_E_BUSY - Search is in progress\n
///
//////////////////////////////////////////////////////////////////////////////
SNAPI 	SNRESULT SNPS3IsScanning();


//////////////////////////////////////////////////////////////////////////////
/// @enum	SNPS3_DISPLAYSETTINGS_MONITORTYPE
///	@brief	Monitor types for SetDisplaySettings
//////////////////////////////////////////////////////////////////////////////
enum SNPS3_DISPLAYSETTINGS_MONITORTYPE
{
	MONITORTYPE_480i = 1,
	MONITORTYPE_576i,
	MONITORTYPE_480p,
	MONITORTYPE_576p,
	MONITORTYPE_1080i_480p,
	MONITORTYPE_1080i_576p,
	MONITORTYPE_1080i_720p_480p,
	MONITORTYPE_1080i_720p_576p,
	MONITORTYPE_1080p_720p_480p,
	MONITORTYPE_1080p_720p_576p,
	MONITORTYPE_WXGA,
	MONITORTYPE_SXGA_WXGA,
	MONITORTYPE_WUXGA_SXGA_WXGA,
	MONITORTYPE_3D_1080p_720p_480p,
	MONITORTYPE_3D_1080p_720p_576p
};

//////////////////////////////////////////////////////////////////////////////
/// @enum	SNPS3_DISPLAYSETTINGS_STARTUPRESOLUTION
///	@brief	Startup resolutions for SetDisplaySettings
//////////////////////////////////////////////////////////////////////////////
enum SNPS3_DISPLAYSETTINGS_STARTUPRESOLUTION
{
	RES_1080 = 1,
	RES_720 = 2,
	RES_480 = 4,
	RES_576,
	RES_1600x1080,
	RES_1440x1080,
	RES_1280x1080,
	RES_960x1080
};

//////////////////////////////////////////////////////////////////////////////
/// @enum	SNPS3_DISPLAYSETTINGS_CONNECTORTYPE
///	@brief	Connector types for SetDisplaySettings
//////////////////////////////////////////////////////////////////////////////
enum SNPS3_DISPLAYSETTINGS_CONNECTORTYPE
{
	CONNECTOR_HDMI = 1,
	CONNECTOR_COMPONENT_D,
	CONNECTOR_COMPOSITE_SVIDEO,
	CONNECTOR_AVMULTI_SCART,
	CONNECTOR_AVMULTI_SCART_RGB,
	CONNECTOR_VGA_DSUB
};

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Helper function to check the display settings validity
///
/// @param		uMonitorType	Monitor type (see \link PS3TMAPI::SNPS3_DISPLAYSETTINGS_MONITORTYPE\endlink).
/// @param		uStartupResolution	Startup resolution (see \link PS3TMAPI::SNPS3_DISPLAYSETTINGS_STARTUPRESOLUTION\endlink).
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///				SN_E_BAD_PARAM - Problem with the provided settings  
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3IsValidResolution(UINT32 uMonitorType, UINT32 uStartupResolution);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Set the display settings of the target
///
/// @param		hTarget	Target
/// @param		szExecutable	Path to a custom setmonitor.self. Can be NULL or empty, that case the builtin will be used 
/// @param		uMonitorType	Monitor type (see \link PS3TMAPI::SNPS3_DISPLAYSETTINGS_MONITORTYPE\endlink).
/// @param		uConnectorType	Connector type (see \link PS3TMAPI::SNPS3_DISPLAYSETTINGS_CONNECTORTYPE\endlink).
/// @param		uStartupResolution	Startup resolution (see \link PS3TMAPI::SNPS3_DISPLAYSETTINGS_STARTUPRESOLUTION\endlink).
/// @param		bHDCP	High-bandwidth Digital Content Protection
/// @param		bResetAfter	Reset the target after the command
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///             SN_E_BUSY - Target is busy\n
///				SN_E_BAD_PARAM - Problem with the provided settings  
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3SetDisplaySettings(HTARGET hTarget, const char* szExecutable, UINT32 uMonitorType, UINT32 uConnectorType, UINT32 uStartupResolution, bool bHDCP, bool bResetAfter);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Map the Playstation3 file system
///
/// @param		driveLetter     The drive letter used to map the Playstation3 file system.
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///				SN_S_NO_ACTION - File System is already mapped to the same drive letter
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3MapFileSystem(char driveLetter);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Unmap the Playstation3 file system
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///				SN_S_NO_ACTION - FileSystem was not mapped
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3UnmapFileSystem();

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Return the Playstation3 file system drive letter
///
/// @param		driveLetter     The drive letter the Playstation3 file system is mapped to.
///                             Value is 0 if the file system is not mapped.
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3GetFileSystem(char* driveLetter);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Import the target settings from a file (Server settings only)
///
/// @details	Imports server settings only. Reset parameters etc are not included
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///				SN_E_BAD_PARAM - FileName is NULL or empty
///				SN_E_FILE_ERROR - Failed to open or load the file
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3ImportTargetSettings(HTARGET hTarget, const char* szFileName);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Export the target settings to a file (Server settings only)
///
/// @details	Exports server settings only. Reset parameters etc are not included
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///				SN_E_BAD_PARAM - FileName is NULL or empty   
///				SN_E_FILE_ERROR - Failed to open or save the file
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3ExportTargetSettings(HTARGET hTarget, const char* szFileName);

/////////////////////////////////////////////////////////////////////////////////
///
/// @brief		Extract data from param.sfo file
///
/// @details	Extracts game parameter from a param.sfo file. Supported keys are
///             "TITLE_ID", "TITLE", "VERSION" and "CATEGORY"
///
///  @param		szFileName		Path to param.sfo file.
///  @param		szKey			Type of data to read ("TITLE_ID", "TITLE", ...)
///  @param		puBufferSize	[in] Maximum number of bytes buffer can hold.\n
///                             [out] Number of bytes for which space is required, 
///                             or if buffer is big enough, number written to buffer.
///  @param		pBuffer			Pointer to buffer where data will be written.
///
/// @note		Reference tool (DECR-1000 / DECR-1000A)							: SDK 3.4.0 - latest version.\n
///             Debugging station (DECHA00J / DECHA00A / DECHJ00J / DECHJ00A)	: SDK 3.4.0 - latest version.\n
///             Reference tool (DECR-1400J / DECR-1400A)						: SDK 3.4.0 - latest version.
///
///	 @return	\link ApiBase::SNRESULT SNRESULT\endlink:\n
///             SN_S_OK - Success\n
///             SN_E_DLL_NOT_INITIALISED - Comms not initialised. Call SNPS3InitTargetComms() first.\n
///				SN_E_BAD_PARAM - szFileName/szKey is NULL or empty   
///             SN_E_OUT_OF_MEM - Number of bytes in bufferSize is too small.\n
///				SN_E_FILE_ERROR - Failed to retreive the data, use SNPS3GetErrorQualifier to get error details
///								- Qualifier ENOEXEC means the file is not a valid param.sfo file
///								- Qualifier EILSEQ means the key was not found inside the param.sfo file
//////////////////////////////////////////////////////////////////////////////
SNAPI SNRESULT SNPS3ExtractGameParameter(const char* szFileName, const char* szKey, UINT32* puBufferSize, BYTE* pBuffer);

#ifdef __cplusplus
}
#endif

#endif // #ifndef PS3TMAPI_H_INC
