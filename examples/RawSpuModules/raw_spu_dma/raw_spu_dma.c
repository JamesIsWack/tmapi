/*   SCE CONFIDENTIAL                                                           */
/*   Cell Evaluation System 030.005                                             */
/*   Copyright (C) 2004 Sony Computer Entertainment Inc.                        */
/*   All Rights Reserved.                                                       */
/* File: raw_spu_dma.h                                                          *
*  Description: SN Systems modified version of SDK sample to demonstrate raw    *
*               spu module debugging                                            *
*               
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <sys/spu_initialize.h>
#include <sys/raw_spu.h>
#include <sys/spu_utility.h>
#include <sys/ppu_thread.h>
#include <sys/interrupt.h>
#include <sys/raw_spu.h>
#include "raw_spu_mmio.h"
#include <libsn.h>

extern const char _binary_spu_module_1_bin_start[];
extern const char _binary_spu_module_1_bin_end[];
extern const char _binary_spu_module_1_bin_size[];

extern const char _binary_spu_module_2_bin_start[];
extern const char _binary_spu_module_2_bin_end[];
extern const char _binary_spu_module_2_bin_size[];

extern const char _binary_spu_loader_elf_start[];
extern const char _binary_spu_loader_elf_end[];
extern const char _binary_spu_loader_elf_size[];

/*
 * Macros to obtain the upper and lower 32 bits of a 64-bit effective address
 */ 
#define EAH(ea64) (uint32_t)((uint64_t)(ea64) >> 32)
#define EAL(ea64) (uint32_t)((uint64_t)(ea64) & 0xFFFFFFFFUL)

/*
 * EIEIO: Guarantees the previous register access have completed. 
 */
#define EIEIO __asm__ volatile("eieio")

#define SPU_PROG  "/app_home/spu_loader.elf"
#define DMA_TAG      0
#define DMA_PUT   0x20
#define BUF_SIZE   128  /* 128 bytes buffer */

////////////////////////////////////////////////////////////////////////////////
typedef union SpuStatusRegister
{
	struct
	{
		uint32_t	m_sc								: 16;
		uint32_t	m_reserved2							: 5;
		uint32_t	m_isolateExitStatus					: 1;
		uint32_t	m_isolateLoadStatus					: 1;
		uint32_t	m_reserved1							: 1;
		uint32_t	m_isolationStatus					: 1;
		uint32_t	m_illegalChannelInstructionDetected	: 1;
		uint32_t	m_invalidInstructionDetected		: 1;
		uint32_t	m_singleStepStatus					: 1;
		uint32_t	m_waitStatus						: 1;
		uint32_t	m_haltStatus						: 1;
		uint32_t	m_programStopAndSignalStatus		: 1;
		uint32_t	m_runStatus							: 1;
	};
	uint32_t	m_val;
} SpuStatusRegister;

uint32_t GetStopSignal( sys_raw_spu_t idSpu )
{
	SpuStatusRegister status;
	status.m_val = mmio_read_prob_reg(idSpu, SPU_Status);
	uint32_t stopSignal = status.m_sc;
	return stopSignal;
}

void handle_syscall (uint64_t arg)
{
	sys_raw_spu_t id = arg;
	uint64_t stat;
	int ret;

	snRawSPULockHandler();
	/*
	* Create a tag to handle class 2 interrupt, because PU Interrupt MB is 
	* handled by class 2. 
	*/
	ret = sys_raw_spu_get_int_stat(id, 2, &stat);
	if (ret)
	{
		printf("raw_spu_get_int_stat is failed %d\n", ret);
		snRawSPUUnlockHandler();
		sys_interrupt_thread_eoi();
	}

	if (stat & 2)	//stop
	{
		//We've hit a stop, so what kind of value is it?
		uint32_t	signalVal = GetStopSignal( id );
		switch ( signalVal )
		{
		case 0x3:

			// it was a stop that is in the SPU code to signal to the PPU

			// do any processing for the user defined stop here
			// if you do not restart the SPU then call snRawSPUNotifySPUStopped(id) to inform the debugger that SPU has stopped

			//restart the SPU
			mmio_write_prob_reg( id, SPU_RunCntl, 0x1 );
			break;
		default:
			snRawSPUNotifySPUStopped(id);
			break;
		}
	}
	else if (stat & 4)	//halt
	{
		snRawSPUNotifySPUStopped(id);
	}

	/*
	* Must reset interrupt status bit of those not handled.  
	*
	*/
	ret = sys_raw_spu_set_int_stat(id, 2, stat);
	if (ret)
	{
		printf("raw_spu_set_int_stat is failed %d\n", ret);
		snRawSPUUnlockHandler();
		sys_interrupt_thread_eoi();
	}

	/*
	* End of interrupt
	*/
	snRawSPUUnlockHandler();

	sys_interrupt_thread_eoi();
}

/* 
 * DMA buffers that are aligned to 128 bytes 
 */
uint32_t dma_recv_buf[BUF_SIZE / sizeof(uint32_t)]__attribute__((aligned(128)));

sys_interrupt_tag_t intrtag;
sys_ppu_thread_t handler;
sys_interrupt_thread_handle_t ih;
#define PRIO                2100
#define PPU_STACK_SIZE    0x4000

int main(int argc, char **argv)
{
    sys_raw_spu_t id;    
	sys_spu_image_t spu_loader_img;
    uint32_t lsa;
    unsigned int i;
    int ret;
    
	unsigned int* pPassingData;
	
	if (snInit() != SUCCEEDED)
	{
		fprintf(stderr, "Failed to initialize libsn\n");
		exit(1);
	}

	/*
     * Initialize SPUs
     */
    if ((ret = sys_spu_initialize(6, 1)) != SUCCEEDED) {
        fprintf(stderr, "spu_initialize returned %d\n", ret);
        fprintf(stderr, "Exiting...\n");
        exit(1);
    }
     
	/*
	*  Create a Raw SPU
	*/
	printf("Creating a Raw SPU...\n");
	if ((ret = sys_raw_spu_create(&id, NULL)) != SUCCEEDED)
	{
		fprintf(stderr, "Fail: raw_spu_create faild.\n");
		fprintf(stderr, "Have you really called spu_initialize in advance?\n");
		fprintf(stderr, "Exiting.\n");
		exit(1);
	}
	printf("Raw SPU %u is created.\n", id);
	snRawSPUNotifyCreation(id);

	/*
     * Loading an SPU program to the Raw SPU.
     */
	ret = sys_spu_image_import(&spu_loader_img, (const void*) _binary_spu_loader_elf_start, SYS_SPU_IMAGE_DIRECT);

    printf("Loading %s to the Raw SPU\n", SPU_PROG);
    if ((ret = sys_raw_spu_image_load(id, &spu_loader_img)) != SUCCEEDED) {
        fprintf(stderr, "Fail: raw_spu_load returned %d\n", ret);
        fprintf(stderr, "Exiting...\n");
        sys_raw_spu_destroy(id);
        exit(1);
    } else {
        printf("Successfully loaded.\n");
    }

	snRawSPUNotifyElfLoad(id, spu_loader_img.entry_point, SPU_PROG);

	printf("syscall pu_thread_create...");
	ret = sys_ppu_thread_create(&handler, handle_syscall, 0, PRIO, PPU_STACK_SIZE, SYS_PPU_THREAD_CREATE_INTERRUPT, "Interrupt PPU Thread");
	if (ret)
	{
		printf("pu_thread_create is failed %d\n", ret);
	}
	printf("succeeded.\n");

	printf("syscall raw_spu_create_intr_tag...");
	ret = sys_raw_spu_create_interrupt_tag(id, 2, SYS_HW_THREAD_ANY, &intrtag);
	if (ret)
	{
		printf("raw_spu_create_intr_tag failed %d\n", ret);
		return -1;
	}
	printf("succeeded. \n");

	printf("syscall intr_thread_establish...");
	ret = sys_interrupt_thread_establish(&ih, intrtag, handler, id);
	if (ret)
	{
		printf("intr_thread_establish is failed %d\n", ret);
		return -1;
	}
	printf("succeeded. \n");

	/*
	* Set interrupt mask.
	* The third argument = 7 enables Halt, Stop-and-Signal and PU Mailbox 
	* interrupts.
	*/
	printf("syscall raw_spu_set_int_mask...");
	ret = sys_raw_spu_set_int_mask(id, 2, 6);
	if (ret)
	{
		printf("failed %d\n", ret);
		return -1;
	}
	printf("succeeded. \n");

	//Pass module base addresses to SPU (Uses hardcoded locations at beginning of SPU LS).
	pPassingData = (unsigned int*)(size_t )(LS_BASE_ADDR( id ) + 0);
	*pPassingData = (unsigned int)(size_t )_binary_spu_module_1_bin_start;
	pPassingData = (unsigned int*)(size_t )(LS_BASE_ADDR( id ) + 4);
	*pPassingData = (unsigned int)(((size_t )_binary_spu_module_1_bin_size + 127) & ~127);

	pPassingData = (unsigned int*)(size_t )(LS_BASE_ADDR( id ) + 8);
	*pPassingData = (unsigned int)(size_t )_binary_spu_module_2_bin_start;
	pPassingData = (unsigned int*)(size_t )(LS_BASE_ADDR( id ) + 12);
	*pPassingData = (unsigned int)(((size_t )_binary_spu_module_2_bin_size + 127) & ~127);

    /* 
     * Run the Raw SPU 
     */
	printf("Run Raw SPU %u\n", id);

	snRawSPUNotifySPUStarted(id);
	mmio_write_prob_reg(id, SPU_NPC, spu_loader_img.entry_point);
	mmio_write_prob_reg(id, SPU_RunCntl, 0x1);
	EIEIO;
	printf("Raw SPU %u started\n", id);

	snPause();

	/*
	* The SPU notifies the DMA buffer LS address via the PU  Mailbox
	* The 31st bit of the MB_Stat register indicates the status of the
	* PU Mailbox. (0: is empty, 1: contains a new value)
	* Poll MB_Stat register until it indicates the PU Mailbox stores a new 
	* value.
	*/
	printf("Read PU_MB of Raw SPU %d\n", id);
	while (!(mmio_read_prob_reg(id, MB_Stat) & 0x1));
	lsa = mmio_read_prob_reg(id, PU_MB);
	printf("Obtained lsa = %#x\n", lsa);

	/*
	* Set the DMA parameters to appropriate registers via MMIO.
	* Check the DMA command status by reading the DMA_CMDStatus register.
	* If the status is 0, the DMA command enqueue has succeeded. Otherwise,
	* the DMA parameter must be set again. 
	*/
	unsigned int dma_stat = 1;
	
	printf("Setting the DMA parameters to the problem registers.\n");
	/* Set the DMA command parameter registers */
	mmio_write_prob_reg(id, DMA_LSA, lsa);
	mmio_write_prob_reg(id, DMA_EAH, EAH(dma_recv_buf));
	mmio_write_prob_reg(id, DMA_EAL, EAL(dma_recv_buf));
	mmio_write_prob_reg(id, DMA_Size_Tag, BUF_SIZE << 16 | DMA_TAG);
	
	do {
		mmio_write_prob_reg(id, DMA_Class_CMD, DMA_PUT);
	    
		/*
		* Check the DMA command status, if it is 0, the command enqueue was
		* successful.  Otherwise, the parameter must be set, agian.
		*/
		dma_stat = mmio_read_prob_reg(id, DMA_CMDStatus);
	} while (dma_stat != 0);
	
	/* 
	* Set a mask for the DMA tag query, and query the tag-group status.
	* The status of DMA of all tags is masked by the value in Prxy_QueryMask,
	* and reflected in Prxy_TagStatus.  Since this program uses only tag 0,
	* unmask the 31th (least-significant) bit.
	*/
	unsigned int tag_mask = 0x1 << DMA_TAG;
	mmio_write_prob_reg(id, Prxy_QueryMask, tag_mask);
	
	/*
	* Polling until Prxy_TagStatus becomes 1.
	*/
	do {
		EIEIO;
	} while ((mmio_read_prob_reg(id, Prxy_TagStatus) & tag_mask) == 0);

	// Output the recv buffer. 
	// It should contain the result from 2 spu modules in the first 4 bytes 
	// followed by 0xFEEDBEEF in the remaining entries.

	printf("printing dma_recv_buf\n");
	for (i = 0; i < BUF_SIZE / sizeof(uint32_t); i++) {
		printf("dma_recv_buf[%d] = %#x\n", i, dma_recv_buf[i]);
	}

    /*
     * Destroy the Raw SPU
     */
    printf("Destroying Raw SPU %d\n", id);

	snRawSPUNotifyDestruction(id);

    if ((ret = sys_raw_spu_destroy(id)) != SUCCEEDED) {
        fprintf(stderr, "raw_spu_destroy returned %d\n", ret);
        fprintf(stderr, "Exiting...\n");
        exit(1);
    }
   
	printf("that's it!\n");
    return 0;
}

