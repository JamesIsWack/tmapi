
#include <cstdlib>
#include <cstring>
#include <cell/cell_fs.h>
#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sys/timer.h>


void load_modules()
{
	int ret = cellSysmoduleLoadModule(CELL_SYSMODULE_FS);

	if (ret)
	{
		printf("cellSysmoduleLoadModule() error 0x%x !\n", ret);
		sys_process_exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
    load_modules();

	int pipe = -1;
	CellFsErrno err = cellFsOpen("/app_home/\\\\.\\pipe\\NamedPipeExample.pipe", CELL_FS_O_RDWR, &pipe, NULL, 0);

	if (err == CELL_FS_SUCCEEDED)
	{
		char msg[] = "Hello World!\n";
		uint64_t tx;

		cellFsWrite(pipe, msg, std::strlen(msg)+1, &tx);

		cellFsClose(pipe);
	}

	return EXIT_SUCCESS;
}
