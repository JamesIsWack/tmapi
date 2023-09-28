/************************************************************************/
/* libsn.h                                                              */
/* ProDG for PlayStation3 Support Library                               */
/*                                                                      */
/* Copyright (c) 2005-2006 SN Systems Ltd.                              */
/* support@snsys.com                                                    */
/*                                                                      */
/************************************************************************/
#ifndef __LIBSN_H__
#define __LIBSN_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LIBSN_VERSION	24
extern unsigned int __SN_Libsn_version[];	/* Declared as array so that compiler long refs this symbol */

/* Example printf("libsn version = %d\n",__SN_Libsn_version[0]); */

int snInit (void);

void snRawSPULockHandler (void);
void snRawSPUUnlockHandler (void);
void snRawSPUNotifyCreation (unsigned int uID);
void snRawSPUNotifyDestruction (unsigned int uID);
void snRawSPUNotifyElfLoad (unsigned int uID, unsigned int uEntry, const char *pFileName);
void snRawSPUNotifyElfLoadNoWait (unsigned int uID, unsigned int uEntry, const char *pFileName);
void snRawSPUNotifyElfLoadAbs (unsigned int uID, unsigned int uEntry, const char *pFileName);
void snRawSPUNotifyElfLoadAbsNoWait (unsigned int uID, unsigned int uEntry, const char *pFileName);
void snRawSPUNotifySPUStopped (unsigned int uID);
void snRawSPUNotifySPUStarted (unsigned int uID);
void snAddPPUPrxSearchPath(const char *pPath);
void snAddSPUElfSearchPath(const char *pPath);
void snAddPathMapping(const char *path, const char *replacementPath);

int snIsDebuggerRunning (void);

char snGetLoadRequest (void);
void snSafeLoadPoint (void);

// For backwards compatibility:
#define snIsDebuggerPresent snIsDebuggerRunning

#ifdef __SNC__
#define snPause() __builtin_snpause()
#else
#define snPause() __asm__ volatile ( "tw 31,1,1" )
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIBSN_H__ */
