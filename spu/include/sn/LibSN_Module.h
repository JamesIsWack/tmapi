/************************************************************************/
/* libsn_module                                                         */
/* ProDG for PlayStation3 Support Library                               */
/*                                                                      */
/* Copyright (c) 2005 SN Systems Ltd.                                   */
/* support@snsys.com                                                    */
/*                                                                      */
/************************************************************************/
#ifndef __LIBSN_SPU_MODULE_H__
#define __LIBSN_SPU_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Raw spu module header */
typedef struct
{
	unsigned int mark1; /* Marker to find module */
	unsigned int rand1; /* GUID to identify */
	unsigned int rand2; /* GUID to identify */
	unsigned int mark2; /* Marker to find module */
	unsigned int entry; /* Address of entry point, as defined in linker script */
	unsigned int bss_start; /* Start address of BSS */
	unsigned int bss_size; /* Size of BSS (loader code has to clear this) */
	unsigned int pad; /* Not used */
} spu_mod_hdr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIBSN_SPU_MODULE_H__ */
