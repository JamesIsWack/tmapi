/*   SCE CONFIDENTIAL                                                           */
/*   Cell Evaluation System 030.005                                             */
/*   Copyright (C) 2004 Sony Computer Entertainment Inc.                        */
/*   All Rights Reserved.                                                       */
/* File: raw_spu_dma.h                                                          *
*  Description: SN Systems modified version of SDK sample to demonstrate        *
*               threaded spu module debugging                                   *
*               
*/ 

#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include <sys\spu_thread.h>
#include <libsn_spu.h>

typedef qword uint128_t;

#define BUF_SIZE  1280

unsigned int dma_buffer[BUF_SIZE / sizeof(unsigned int)]__attribute__((aligned(128)));
unsigned int code1Buffer[BUF_SIZE / sizeof(unsigned int)]__attribute__((aligned(128)));
unsigned int code2Buffer[BUF_SIZE / sizeof(unsigned int)]__attribute__((aligned(128)));

void DmaIn( void* spu_addr, unsigned int ydram_addr, unsigned int length, unsigned int id )
{
	spu_mfcdma64( spu_addr, 0, ydram_addr, length, id, MFC_GET_CMD);
}

void DmaStall( void )
{
	spu_writech(MFC_WrTagMask, 0xFFFFFFFF);
	spu_mfcstat(2);
}

int main(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
	//Read pointers passed from PPU.
	unsigned int code1Addr = arg1;
	unsigned int code1Size = arg2;
	unsigned int code2Addr = arg3;
	unsigned int code2Size = arg4;

	//DMA modules into SPU memory...
	DmaIn( code1Buffer, code1Addr, code1Size, 0 );
	DmaIn( code2Buffer, code2Addr, code2Size, 1 );
	DmaStall();

	//Get pointers to module entry points...
	typedef unsigned int (*LoadedFunc)( void );

	unsigned int uCode1Entry = (unsigned int)code1Buffer + 0x10;// + sizeof spuGUID section
	unsigned int uCode2Entry = (unsigned int)code2Buffer + 0x10;// + sizeof spuGUID section

	LoadedFunc loadedFuncPtr1 = (LoadedFunc) (uCode1Entry);
	LoadedFunc loadedFuncPtr2 = (LoadedFunc) (uCode2Entry);

	//Call modules...
	unsigned int ret1 = loadedFuncPtr1();
	unsigned int ret2 = loadedFuncPtr2();

	//Add up the return values...
	unsigned int total = ret1 + ret2;

	//Return result to ppu in exit status
	sys_spu_thread_exit(total);
    return 0;
}
