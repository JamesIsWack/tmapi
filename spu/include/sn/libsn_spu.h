/************************************************************************/
/* libsn_spu.h                                                          */
/* ProDG for PlayStation3 Support Library                               */
/*                                                                      */
/* Copyright (c) 2005 SN Systems Ltd.                                   */
/* support@snsys.com                                                    */
/*                                                                      */
/************************************************************************/
#ifndef __LIBSN_SPU_H__
#define __LIBSN_SPU_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "LibSN_Module.h"

#define snPause() __asm volatile ("stopd 0,1,1")

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIBSN_SPU_H__ */
