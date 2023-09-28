/*   SCE CONFIDENTIAL                                                           */
/*   Cell Evaluation System 030.005                                             */
/*   Copyright (C) 2004 Sony Computer Entertainment Inc.                        */
/*   All Rights Reserved.                                                       */
/* File: raw_spu_dma.h                                                          *
*  Description: SN Systems modified version of SDK sample to demonstrate raw    *
*               spu module debugging                                            *
*               
*/ 

/* Statically defined base addresses and offsets */

/* Macros to calculate base addresses with the given Raw SPU ID */
#define LS_BASE_ADDR(id) \
(RAW_SPU_OFFSET * id + RAW_SPU_BASE_ADDR + RAW_SPU_LS_OFFSET)
#define PROB_BASE_ADDR(id) \
(RAW_SPU_OFFSET * id + RAW_SPU_BASE_ADDR + RAW_SPU_PROB_OFFSET)

/* Problem State Memory Map Register Offset */
#define DMA_LSA           0x3004
#define DMA_EAH           0x3008
#define DMA_EAL           0x300C
#define DMA_Size_Tag      0x3010
#define DMA_Class_CMD     0x3014
#define DMA_CMDStatus     0x3014
#define DMA_QStatus       0x3104
#define PU_MB             0x4004
#define SPU_MB            0x400C
#define MB_Stat           0x4014
#define Sig_Notify_1     0x1400C
#define Sig_Notify_2     0x1C00C

/* A macro to write to a problem state memory map register specified by
 * the given Raw SPU ID and register offset.
 *   Usage: write_reg(1, SPU_MB, 0x1);
 *
 *   \param   id       Raw SPU ID
 *   \param   offset   Register Offset
 *   \param   value    Value to write 
 */ 
#define mmio_write_prob_reg(id, offset, value) \
*(volatile uint32_t*)get_reg_addr(id, offset) = value

/* A macro to read from a problem state memory map register specifid by 
 * the given Raw SPU ID and register offset.  
 *   Usage: uint32_t val = read_reg(1, PU_MB);
 *
 *   \param   id       Raw SPU ID
 *   \param   offset   Register Offset
 *   \return  The register value
 */
#define mmio_read_prob_reg(id, offset) \
(*(volatile uint32_t*)get_reg_addr(id, offset))


