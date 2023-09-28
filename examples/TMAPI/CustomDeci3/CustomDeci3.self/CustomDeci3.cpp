/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007  Sony Computer Entertainment Inc. / SN Systems Ltd
//
/////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cstdio>
#include <memory>

#include <cell/sysmodule.h>
#include <sys/deci3.h>
#include <sys/event.h>
#include <sysutil/sysutil_common.h>

#include <libsn.h>

sys_deci3_protocol_t const DEF_PROTOCOL_NUM = 0x1001;
sys_deci3_port_t const DEF_PORT_NUM = 0;
uint32_t const TX_SIZE = 1024;

bool process_event(sys_event_t& event, sys_deci3_session_t& session, uint8_t* buffer, uint32_t buff_size);
void sysutil_callback(uint64_t status, uint64_t param, void* usr);

int main()
{
	// Load all modules
	if (CELL_OK != cellSysmoduleLoadModule(CELL_SYSMODULE_FS))
	{
		std::puts("FAIL: Load Modules");
		return EXIT_FAILURE;
	}

	// Create the deci3 session
	sys_deci3_session_t session;

	if (SYS_DECI3_OK != sys_deci3_open(DEF_PROTOCOL_NUM, DEF_PORT_NUM, NULL, &session))
	{
		std::puts("FAIL: Initialize session");
		return EXIT_FAILURE;
	}

	// Create the event queue
	sys_event_queue_t queue;
	sys_event_queue_attribute_t attrib = {SYS_SYNC_PRIORITY, SYS_PPU_QUEUE};

	int err = sys_event_queue_create(&queue, &attrib, SYS_EVENT_QUEUE_LOCAL, SYS_DECI3_DEFAULT_QUEUE_SIZE);
	if (err != CELL_OK)
	{
		std::puts("FAIL: Initialize event queue");
		return EXIT_FAILURE;
	}

	// Associate the queue with the session
	if (SYS_DECI3_OK != sys_deci3_create_event_path(session, TX_SIZE, queue))
	{
		std::puts("FAIL: Initialize event path");
		return EXIT_FAILURE;
	}

	// Allocate a buffer for tx/rx
	uint8_t buffer[TX_SIZE] = {};

	// Process messages
	bool ok = false;
	bool quit = false;

	cellSysutilRegisterCallback(0, sysutil_callback, &quit);

	do
	{
		ok = false;

		uint32_t const timeout = 500000;
		sys_event_t event;

		int ret = sys_event_queue_receive(queue, &event, timeout);

		switch (ret)
		{
		case CELL_OK:
			ok = process_event(event, session, buffer, sizeof(buffer));
			break;
		case ETIMEDOUT:
			cellSysutilCheckCallback(); // May cause `quit' to be modified.
			ok = !quit;
			break;
		default:
			break;
		}

	} while (ok);

	// Clean-up
	sys_deci3_close(session);
	sys_event_queue_destroy(queue, 0);

	// Exit
	std::puts("Exiting");
	std::fflush(stdout);

	return (ok) ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool process_event(sys_event_t& event, sys_deci3_session_t& session, uint8_t* buffer, uint32_t buff_size)
{
	switch (event.data1)
	{
	case SYS_DECI3_EVENT_COMM_ENABLED:
		std::puts("Communication enabled");
		return true;

	case SYS_DECI3_EVENT_COMM_DISABLED:
		std::puts("Communication disabled");
		return true;

	case SYS_DECI3_EVENT_DATA_READY:
		if (event.data2 > buff_size)
		{
			std::puts("FAIL: Insufficient space for received packet");
			return false;
		}
		else
		{
			if (SYS_DECI3_OK == sys_deci3_receive(session, buffer, static_cast<size_t>(event.data2)))
			{
				// Just send the message back to the host
				return CELL_OK == sys_deci3_send(session, buffer, static_cast<size_t>(event.data2));
			}

			return false;
		}
	case SYS_DECI3_EVENT_TX_FAILED:
		std::puts("FAIL: Transaction failed");
		return true;

	default:
		break;
	}

	return false;
}

void sysutil_callback(uint64_t status, uint64_t /*param*/, void* usr)
{
	bool& quit = *static_cast<bool*>(usr);

	switch (status)
	{
	case CELL_SYSUTIL_REQUEST_EXITGAME:
		quit = true;
		break;
	}
}
