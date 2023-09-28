#include "util.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/integertypes.h>
#include <cell/codec.h>
#include <cell/sysmodule.h>

static void *pngCbControlMalloc(uint32_t  size, void *cbCtrlMallocArg)
{
	return malloc(size);
}

static int32_t pngCbControlFree(void *ptr, void *cbCtrlFreeArg)
{
	free(ptr);
	return 0;
}

uint8_t *LoadPNG(const char *filename, int *width, int *height)
{
	assert(filename != NULL);
	uint32_t ret = cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
	assert(ret == CELL_OK);
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
	assert(ret == CELL_OK);

	CellPngDecThreadInParam tinParam;
	CellPngDecMainHandle hMain;
	CellPngDecThreadOutParam toutParam;

	tinParam.spuThreadEnable = CELL_PNGDEC_SPU_THREAD_DISABLE;
	tinParam.ppuThreadPriority = 500;
	tinParam.cbCtrlMallocFunc = pngCbControlMalloc;
	tinParam.cbCtrlFreeFunc = pngCbControlFree;

	ret = cellPngDecCreate(&hMain, &tinParam, &toutParam);
	assert(ret == CELL_OK);

	CellPngDecSrc source;
	CellPngDecSubHandle hSub;
	CellPngDecOpnInfo openInfo;
	source.srcSelect = CELL_PNGDEC_FILE;
	source.fileName = filename;
	source.fileOffset = 0;
	source.fileSize = 0;
	source.spuThreadEnable = CELL_PNGDEC_SPU_THREAD_DISABLE;

	ret = cellPngDecOpen(hMain, &hSub, &source, &openInfo);
	assert(ret == CELL_OK);

	CellPngDecInfo info;
	ret = cellPngDecReadHeader(hMain, hSub, &info);
	assert(ret == CELL_OK);

	CellPngDecInParam inParam;
	CellPngDecOutParam outParam;
	inParam.outputMode = CELL_PNGDEC_TOP_TO_BOTTOM;
	inParam.outputColorSpace = CELL_PNGDEC_RGBA;
	inParam.outputBitDepth = 8;
	inParam.outputAlphaSelect = CELL_PNGDEC_FIX_ALPHA;
	inParam.outputColorAlpha = 0xff;
	inParam.outputPackFlag = CELL_PNGDEC_1BYTE_PER_1PIXEL;

	ret = cellPngDecSetParameter(hMain, hSub, &inParam, &outParam);
	assert(ret == CELL_OK);

	uint8_t *pPngData = NULL;
	CellPngDecDataCtrlParam dataCtrl;
	CellPngDecDataOutInfo dataOutInfo;

	pPngData = new uint8_t[info.imageWidth * info.imageHeight * 4];
	*width = info.imageWidth;
	*height = info.imageHeight;

	assert(pPngData != NULL);
	dataCtrl.outputBytesPerLine = info.imageWidth * 4;

	ret = cellPngDecDecodeData(hMain, hSub, pPngData, &dataCtrl, &dataOutInfo);
	assert(ret == CELL_OK);
	assert(dataOutInfo.status == CELL_PNGDEC_DEC_STATUS_FINISH);

	ret = cellPngDecClose(hMain, hSub);
	assert(ret == CELL_OK);
	ret = cellPngDecDestroy(hMain);
	assert(ret == CELL_OK);

	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_PNGDEC);
	assert(ret == CELL_OK);
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);
	assert(ret == CELL_OK);

	return pPngData;
}
