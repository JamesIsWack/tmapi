#ifndef __POLICY_MODULE_H__
#define __POLICY_MODULE_H__

#include <stdint.h>
#include <cell/spurs/types.h>

typedef struct SimplePolicyModule 
{
	uint64_t ea_programs;
	uint32_t num_programs;
	CellSpursWorkloadId id;
	uint64_t ea_spurs;
	uint32_t queue;
	uint8_t port;
	uint8_t debug;			//add in a debug flag

	char padding[128 
		- sizeof(uint64_t)*2 
		- sizeof(uint32_t)*2 
		- sizeof(uint8_t)*2
		- sizeof(CellSpursWorkloadId)];

} SimplePolicyModule __attribute__((__aligned__(128)));

typedef struct SimplePolicyModuleProgram
{
	uint64_t ea_bin;
	uint32_t bin_size;
	uint32_t arg;

} SimplePolicyModuleProgram __attribute__((__aligned__(16)));


#ifdef __SPU__

void pm_main(uintptr_t pKernelContext, uint64_t ea_simple_pm) __attribute__((__noreturn__));

typedef void (*entry_t)(uint32_t arg);

#endif /* __SPU__ */

#endif //#ifndef __POLICY_MODULE_H__
