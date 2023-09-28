/*   SCE CONFIDENTIAL                                                           */
/*   Cell Evaluation System 030.005                                             */
/*   Copyright (C) 2004 Sony Computer Entertainment Inc.                        */
/*   All Rights Reserved.                                                       */
/* File: threaded_spu_dma.h                                                          *
*  Description: SN Systems modified version of SDK sample to demonstrate        *
*               threaded spu module debugging                                   *
*               
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/spu_initialize.h>
#include <sys/spu_utility.h>
#include <sys/spu_image.h>
#include <sys/ppu_thread.h>
#include <sys/interrupt.h>
#include <sys/spu_thread_group.h>

#define NUM_SPU_THREADS	1
#define PRIORITY            100

#define SPU_PROG	"/app_home/spu_loader.elf"

extern const char _binary_spu_loader_elf_start[];
extern const char _binary_spu_loader_elf_end[];
extern const char _binary_spu_loader_elf_size[];

extern const char _binary_spu_module_1_bin_start[];
extern const char _binary_spu_module_1_bin_end[];
extern const char _binary_spu_module_1_bin_size[];

extern const char _binary_spu_module_2_bin_start[];
extern const char _binary_spu_module_2_bin_end[];
extern const char _binary_spu_module_2_bin_size[];

////////////////////////////////////////////////////////////////////////////////

int LoadFile( const char* filename, void **ppData, uint32_t *puSize );

int main(int argc, char **argv)
{
    int ret;

	const char *group_name = "Group";
	sys_spu_thread_group_t group;					/* SPU thread group ID */
	sys_spu_thread_group_attribute_t group_attr;	/* SPU thread group attribute*/
	sys_spu_thread_t thread;						/* SPU thread IDs */
	sys_spu_thread_attribute_t thread_attr;			/* SPU thread attribute */
	sys_spu_image_t spu_img;
	sys_spu_thread_argument_t spu_args = {0};

	/*
     * Initialize SPUs
     */
	printf("Creating an SPU thread group.\n");
	group_attr.name = group_name;
	group_attr.nsize = strlen(group_attr.name) + 1; /* Add 1 for '\0' */
	group_attr.type = SYS_SPU_THREAD_GROUP_TYPE_NORMAL;
	ret = sys_spu_thread_group_create(&group, NUM_SPU_THREADS, PRIORITY, &group_attr);

	thread_attr.name = "Thread";
	thread_attr.nsize = 7;
	thread_attr.option = SYS_SPU_THREAD_OPTION_NONE;

	/*
     * Loading an SPU program to the SPU.
     */
    printf("Loading %s to the SPU\n", SPU_PROG);
	ret = sys_spu_image_import(&spu_img, (const void*) _binary_spu_loader_elf_start, SYS_SPU_IMAGE_DIRECT);

	if (ret != CELL_OK)
	{
		fprintf(stderr, "sys_spu_image_open failed: %#.8x\n", ret);
		exit(ret);
	}
    else
	{
        printf("Successfully loaded.\n");
    }


/*
	//Load the two ELF modules...
	if (LoadFile( SPU_MODULE1 ,&loadedAddress1, &loadedSize1) == 0)
		return -1;

	if (LoadFile( SPU_MODULE2 ,&loadedAddress2, &loadedSize2) == 0)
		return -1;
*/

	//Pass the address & size of each module to the spu loader via args
	spu_args.arg1 = SYS_SPU_THREAD_ARGUMENT_LET_32((unsigned int)_binary_spu_module_1_bin_start);
	spu_args.arg2 = SYS_SPU_THREAD_ARGUMENT_LET_32((unsigned int)(((size_t )_binary_spu_module_1_bin_size + 127) & ~127));
	spu_args.arg3 = SYS_SPU_THREAD_ARGUMENT_LET_32((unsigned int)_binary_spu_module_2_bin_start);
	spu_args.arg4 = SYS_SPU_THREAD_ARGUMENT_LET_32((unsigned int)(((size_t )_binary_spu_module_2_bin_size + 127) & ~127));

	//Initialise the spu
	ret = sys_spu_thread_initialize(&thread, group, 0, &spu_img, &thread_attr, &spu_args);
	if (ret != CELL_OK)
	{
		fprintf(stderr, "sys_spu_thread_initialize failed: %#.8x\n", ret);
		exit(ret);
	}

	printf("Starting the SPU thread group.\n");
	ret = sys_spu_thread_group_start(group);
	if (ret != CELL_OK)
	{
		fprintf(stderr, "sys_spu_thread_group_start failed: %#.8x\n", ret);
		exit(ret);
	}

	printf("Waiting for the SPU thread group to be terminated.\n");
	int cause, status;
	ret = sys_spu_thread_group_join(group, &cause, &status);
	if (ret != CELL_OK) {
		fprintf(stderr, "sys_spu_thread_group_join failed: %#.8x\n", ret);
		exit(ret);
	}

	switch(cause) {
	case SYS_SPU_THREAD_GROUP_JOIN_GROUP_EXIT:
		printf("The SPU thread group exited by sys_spu_thread_group_exit().\n");
		printf("The group's exit status = %d\n", status);
		break;
	case SYS_SPU_THREAD_GROUP_JOIN_ALL_THREADS_EXIT:
		{
			printf("All SPU thread exited by sys_spu_thread_exit().\n");
			int thr_exit_status;
			ret = sys_spu_thread_get_exit_status(thread, &thr_exit_status);
			if (ret != CELL_OK) {
				fprintf(stderr, "sys_spu_thread_get_exit_status failed:%.8x\n", ret);
			}
			printf("Result from spu 0x%02x (via exit status) = %d\n", thread, thr_exit_status);
		}
		break;
	case SYS_SPU_THREAD_GROUP_JOIN_TERMINATED:
		printf("The SPU thread group is terminated by sys_spu_thread_terminate().\n");
		printf("The group's exit status = %d\n", status);
		break;
	default:
		fprintf(stderr, "Unknown exit cause: %d\n", cause);
		break;
	}

	/*
	* Destroy the SPU thread group and clean up resources.
	*/
	ret = sys_spu_thread_group_destroy(group);
	if (ret != CELL_OK)
	{
		fprintf(stderr, "sys_spu_thread_group_destroy failed: %#.8x\n", ret);
	}

	/*
	* The opened SPU program image can be closed after the destruction of 
	* SPU threads
	*/
	ret = sys_spu_image_close(&spu_img);
	if (ret != CELL_OK)
	{
		fprintf(stderr, "sys_spu_image_close failed: %.8x\n", ret);
	}

	printf("Exiting.\n");
	return 0;
}

int LoadFile( const char* filename, void **ppData, uint32_t *puSize )
{
	size_t length;
	size_t alloclength;

	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Failed loading %s, (pseudo_fs_get_file_size - reason = %d)\n", filename, 0);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	alloclength = (length + 127) & ~127;

	void* data = memalign( 128, alloclength );

	if (fread(data, length, 1, fp) == 0)
	{
		fclose(fp);
		printf("Failed loading %s (pseudo_fs_read)\n", filename);
		return 0;
	}
	fclose(fp);

	*ppData = data;
	*puSize = alloclength;

	return length;
}

