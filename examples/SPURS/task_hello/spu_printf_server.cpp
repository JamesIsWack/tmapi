/*  SCE CONFIDENTIAL                                                         
 *  PLAYSTATION(R)3 Programmer Tool Runtime Library 150.004                                           
 *  Copyright (C) 2007 Sony Computer Entertainment Inc.                      
 *  All Rights Reserved.                                                     
 *
 *  Description: 
 *  ppu-side "spu_printf" server
 *               
 */ 

#include <stdio.h>
#include <spu_printf.h>
#include <cell/spurs.h>
#include "spu_printf_server.h"

#define	TERMINATE_PORT_NAME		0xFEE1DEAD
#define	STACK_SIZE				(1024 * 64)
#define SPU_PORT_PRINTF			1

void spuPrintfServer::spu_printf_handler_entry(uint64_t arg)
{
	spuPrintfServer *p = (spuPrintfServer *)(uintptr_t) arg;	
	while (p)
	{
		sys_event_t event;

		int ret = sys_event_queue_receive(p->m_event_queue, &event, 0);
		if (event.source == TERMINATE_PORT_NAME) 
		{
			printf("spu_printf_handler_entry exit! (0x%x)\n", (int)p);
			sys_ppu_thread_exit(0);
		}

		sys_spu_thread_t spu = event.data1;

		int sret = spu_thread_printf(spu, (uint32_t)event.data3);
		ret = sys_spu_thread_write_spu_mb(spu, sret);
		if (ret) 
		{
			printf ("sys_spu_thread_write_spu_mb failed (%d)\n", ret);
			sys_ppu_thread_exit((uint64_t)-1);
		}
	}

	printf("spu_printf_handler_entry exit! (0x%x)\n", (int)p);
	sys_ppu_thread_exit(0);
}

int spuPrintfServer::Initialize(int priority, int max_spu_threads)
{
	int	ret;

	// Create an event queue to sink printf events ...
	sys_event_queue_attribute_t	attr;
	sys_event_queue_attribute_initialize (attr);

	ret = sys_event_queue_create (
		&m_event_queue, 
		&attr, 
		SYS_EVENT_QUEUE_LOCAL, 
		127);

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::start() - sys_event_queue_create failed (%d)\n", ret);
		return ret;
	}

	// Create a thread to deal with printf events ...
	ret = sys_ppu_thread_create (
		&m_spu_printf_handler, 
		spu_printf_handler_entry,
		(uint64_t)(uintptr_t)this, 
		priority, 
		STACK_SIZE, 
		SYS_PPU_THREAD_CREATE_JOINABLE, 
		"spu_printf_handler");

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::start() - sys_ppu_thread_create failed (%d)\n", ret);
		return ret;
	}

	// Create a port for terminating this service ...
	ret = sys_event_port_create(
		&m_terminate_port, 
		SYS_EVENT_PORT_LOCAL, 
		TERMINATE_PORT_NAME);

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::start() - sys_event_port_create failed (%d)\n", ret);
		return ret;
	}

	ret = sys_event_port_connect_local(
		m_terminate_port, 
		m_event_queue);

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::start() - sys_event_port_connect_local failed (%d)\n", ret);
		return ret;
	}

	return CELL_OK;
}

int spuPrintfServer::RegisterSpursGroup(CellSpurs *spurs)
{
	uint8_t	port = SPU_PORT_PRINTF;

	int ret = cellSpursAttachLv2EventQueue (
		spurs, 
		m_event_queue, 
		&port, 
		0);

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::RegisterSpursGroup() - cellSpursAttachLv2EventQueue failed (%d)\n", ret);
		return ret;
	}

	return CELL_OK;
}

int spuPrintfServer::UnRegisterSpursGroup(CellSpurs *spurs)
{
	int ret = cellSpursDetachLv2EventQueue(
		spurs, 
		SPU_PORT_PRINTF);

	if (ret)
	{
		fprintf (stderr, "spuPrintfServer::UnRegisterSpursGroup() - cellSpursDetachLv2EventQueue failed (%d)\n", ret);
		return ret;
	}	

	return CELL_OK;
}


int spuPrintfServer::RegisterSpuThread(sys_spu_thread_t spu)
{
	int ret = sys_spu_thread_connect_event(
		spu, 
		m_event_queue,
		SYS_SPU_THREAD_EVENT_USER, 
		SPU_PORT_PRINTF);

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::RegisterSpuThread() - sys_spu_thread_connect_event failed (%d)\n", ret);
		return ret;
	}

	return CELL_OK;
}

int spuPrintfServer::UnRegisterSpuThrad(sys_spu_thread_t spu)
{
	int ret = sys_spu_thread_disconnect_event(
		spu, 
		SYS_SPU_THREAD_EVENT_USER, 
		SPU_PORT_PRINTF);

	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::UnRegisterSpuThread() - sys_spu_thread_disconnect_event failed (%d)\n", ret);
		return -1;
	}

	return CELL_OK;
}

int spuPrintfServer::Destroy()
{
	int ret;

	// Send termination event ...
	ret = sys_event_port_send (m_terminate_port, 0, 0, 0);
	if (ret)
	{
		fprintf (stderr, "spuPrintfServer::Destroy() - sys_event_port_send failed (%d)\n", ret);
		return ret;
	}

	// Wait for handler thread ...
	uint64_t exit_code;
	ret = sys_ppu_thread_join (m_spu_printf_handler, &exit_code);
	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::Destroy() - sys_ppu_thread_join failed (%d)\n", ret);
		return ret;
	}

	// Disconnect and destroy the terminate port ...
	ret = sys_event_port_disconnect(m_terminate_port);
	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::Destroy() - sys_event_port_disconnect failed (%d)\n", ret);
		return ret;
	}
	ret = sys_event_port_destroy(m_terminate_port);
	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::Destroy() - sys_event_port_destroy failed (%d)\n", ret);
		return ret;
	}	

	// Finally, destory the event queue...
	ret = sys_event_queue_destroy (m_event_queue, 0);
	if (ret) 
	{
		fprintf (stderr, "spuPrintfServer::Destroy() - sys_event_queue_destroy failed %d\n", ret);
		return ret;
	}

	return CELL_OK;

}
