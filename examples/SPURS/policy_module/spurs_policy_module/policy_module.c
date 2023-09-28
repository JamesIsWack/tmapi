#include <spu_mfcio.h>
#include <cell/dma.h>
#include <cell/spurs/policy_module.h>
#include <cell/spurs/ready_count.h>
#include <spu_printf.h>
#include <libsn_SPU.h>

#include "policy_module.h"

static const uint32_t tag1 = 3;

static const uintptr_t PROGRAM_START = (16 * 1024);

const uint32_t PM_PROGRAM_FOUND			= 0;
const uint32_t PM_PROGRAM_FOUND_LAST	= 1;
const uint32_t PM_PROGRAM_NOT_FOUND		= 2;

SimplePolicyModule			simple_pm;
SimplePolicyModuleProgram	spu_program;

static
int GetNextProgram(SimplePolicyModuleProgram *program, uint64_t ea_simple_pm)
{
	uint64_t ea;
	int ret;
	do
	{
		cellDmaGetllar((void*)&simple_pm, ea_simple_pm, 0, 0);
		cellDmaWaitAtomicStatus();

		if(simple_pm.num_programs > 0)
		{
			if(simple_pm.num_programs == 1)
			{
				ret = PM_PROGRAM_FOUND_LAST;
			}
			else
			{
				ret = PM_PROGRAM_FOUND;
			}
			
			ea = simple_pm.ea_programs;
			
			simple_pm.ea_programs += sizeof(SimplePolicyModuleProgram);
			simple_pm.num_programs--;
		}
		else
		{
			ret = PM_PROGRAM_NOT_FOUND;
			ea = 0;
		}

		cellDmaPutllc((void*)&simple_pm, ea_simple_pm, 0, 0);

	} while (cellDmaWaitAtomicStatus());

	if (ret != PM_PROGRAM_NOT_FOUND)
	{
		//Get info about next program...
		cellDmaGet((void*)program, ea, sizeof(SimplePolicyModuleProgram), tag1, 0, 0);
		cellDmaWaitTagStatusAll(1<<tag1);

		//Load program code into memory...
		cellDmaGet(
			(void*)PROGRAM_START, 
			program->ea_bin, 
			(program->bin_size+15)& ~15,
			tag1,
			0,
			0);

		cellDmaWaitTagStatusAll(1<<tag1);
	}

	return ret;
}

static 
void ExecuteProgram(SimplePolicyModuleProgram *program)
{
	entry_t entry = (entry_t) PROGRAM_START + 0x10;
	
	spu_sync();
	
	entry(program->arg);
}

void pm_main(uintptr_t pKernelContext, uint64_t ea_simple_pm)
{
	entry_t entry = (entry_t)PROGRAM_START;
	spu_printf("[SPU] Policy module start... (entry=%x)\n", (unsigned int)entry);

	cellDmaGetllar((void*)&simple_pm, ea_simple_pm, 0, 0);
	cellDmaWaitAtomicStatus();

	//Check the debug flag and pause if set for debugging.
	if (simple_pm.debug)
		snPause();

	//Dispatch work until no more available...
	int ret;
	do 
	{
		ret = GetNextProgram(&spu_program, ea_simple_pm);
	
		if((ret == PM_PROGRAM_FOUND) || (ret == PM_PROGRAM_FOUND_LAST))
		{
			ExecuteProgram(&spu_program);
		}

	} while (ret == PM_PROGRAM_FOUND);

	spu_printf("[SPU] Policy module end... (entry=%x)\n", (unsigned int)entry);

	//Signal the PPU that we've finished processing the program chain...
	cellDmaGetllar((void*)&simple_pm, ea_simple_pm, 0, 0);
	cellDmaWaitAtomicStatus();

	sys_spu_thread_throw_event(simple_pm.port, 0, 0);

	cellSpursModuleReadyCountSwap(pKernelContext, 0);
	cellSpursModuleExit(pKernelContext);
}
