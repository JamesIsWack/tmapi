/*   SCE CONFIDENTIAL                                                           */
/*   Cell Evaluation System 030.005                                             */
/*   Copyright (C) 2004 Sony Computer Entertainment Inc.                        */
/*   All Rights Reserved.                                                       */
/* File: raw_spu_dma.h                                                          *
*  Description: SN Systems modified version of SDK sample to demonstrate raw    *
*               spu module debugging                                            *
*               
*/ 

#include <spu_intrinsics.h>
#include <spu_mfcio.h>
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

int main(void)
{
    int i;
	uint128_t uValue128 = si_from_ushort(0);
	uint128_t uArray128[10];

	uValue128 = si_from_uint(0xFEEDBEEF);
	uArray128[0] = uValue128;
	for (i = 1; i < 10; i++)
		uArray128[i] = uArray128[i - 1];

	//Read pointers written by PPU.
	unsigned int code1Addr = *(unsigned int*) 0x0;
	unsigned int code1Size = *(unsigned int*) 0x4;
	unsigned int code2Addr = *(unsigned int*) 0x8;
	unsigned int code2Size = *(unsigned int*) 0xc;

	//Clear the buffer...
    for (i = 0; i < BUF_SIZE / sizeof(unsigned int); i++) {
        dma_buffer[i] = 0xFEEDBEEFUL;
    }

	////////////////////////////////////////////////////////////
	//On seeing an "spu_stop(3)", the interrupt handler will do
	//nothing other than restart this SPU
	//spu_stop(3);
	snPause();
	////////////////////////////////////////////////////////////

	//DMA modules into SPU memory...
	DmaIn( code1Buffer, code1Addr, code1Size, 0 );
	DmaIn( code2Buffer, code2Addr, code2Size, 1 );
	DmaStall();

	//Get pointers to module entry points...
	typedef unsigned int (*LoadedFunc)( void );

	unsigned int uCode1Entry = ((spu_mod_hdr*)code1Buffer)->entry + (unsigned int)code1Buffer;
	unsigned int uCode2Entry = ((spu_mod_hdr*)code2Buffer)->entry + (unsigned int)code2Buffer;

	LoadedFunc loadedFuncPtr1 = (LoadedFunc) (uCode1Entry);
	LoadedFunc loadedFuncPtr2 = (LoadedFunc) (uCode2Entry);

	////////////////////////////////////////////////////////////
	//On seeing an "spu_stop(3)", the interrupt handler will do
	//nothing other than restart this SPU
	//spu_stop(3);

	snPause();
	////////////////////////////////////////////////////////////

	//Call modules...
	unsigned int ret1 = loadedFuncPtr1();
	unsigned int ret2 = loadedFuncPtr2();

	//Add up the return values...
	unsigned int total = ret1 + ret2;

	//Store the result ready for the PPU...
	dma_buffer[0] = total;

    //Send the address of dma_buffer via PPU mailbox...
    spu_writech(SPU_WrOutMbox, (unsigned int)dma_buffer);

    /*
     * Raw SPU keeps allocated in a physical SPU even if it is stopped.
     * Issuing DMA to a stopped SPU is OK. 
     */
	////////////////////////////////////////////////////////////
	//This stop indicates the SPU program has completed successfully
	spu_stop( 0x2000 );
	////////////////////////////////////////////////////////////
    return 0;
}
