/*  SCE CONFIDENTIAL                                                         
 *  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
 *  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
 *  All Rights Reserved.                                                     
 *
 *  Description: 
 *  ppu-side "spu_printf" server
 *               
 */ 

#ifndef _SPU_PRINTF_H_INC_
#define _SPU_PRINTF_H_INC_

class spuPrintfServer
{
public:
	int Initialize(int priority = 200, int max_spu_threads = 127);
	int Destroy();

	int RegisterSpursGroup(CellSpurs *spurs);
	int UnRegisterSpursGroup(CellSpurs *spurs);

	int RegisterSpuThread(sys_spu_thread_t spu);
	int UnRegisterSpuThrad(sys_spu_thread_t spu);

protected:
	static void spu_printf_handler_entry(uint64_t arg);

	bool				m_initialized;
	sys_event_queue_t	m_event_queue;
	sys_ppu_thread_t	m_spu_printf_handler;
	sys_event_port_t	m_terminate_port;

};

#endif //#ifndef _SPU_PRINTF_H_INC_
