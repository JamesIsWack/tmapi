/*
 *   SCE CONFIDENTIAL
 *   Copyright (C) 2011 Sony Computer Entertainment Inc.
 *   All Rights Reserved. 
 */
/**
 * dumpHeapArea.c
 * Description:
 *   This script dumps the information about the heap area which is managed
 *   by malloc in the standard library. The heap area includes some segments
 *   and the segment also includes some chunks. You can see the inuse and
 *   the freed chunk by this script. Note that the address which malloc
 *   returns is associated to a chunk. Therefore, you can detect the
 *   fragmentation in the heap area by checking the freed chunk.
 *
 *   The output is stored into a file you specified by dialogue. If you
 *   cancel to select, the standard output (into Script View) is used.
 *
 * How to use
 *   When the script is executed, the item "Dump Heap Area" is added into
 *   the menu in the "Source View". You can see the dump of the heap area
 *   by selecting this item.
 **/

#define MAGIC_OFFSET (32U)
#define MAGIC_NUMBER (0x58585858U)
#define SEG_OFFSET (464U)
#define NEXT_SEG_OFFSET (8U)

/* static global objects */
static sn_uint32 sInuseSize;
static sn_uint32 sFreedSize;
static sn_uint32 sMaxInuseSize;
static sn_uint32 sMaxFreedSize;
static char filePath[1024];
static char fileName[1024];


/**
 * Get 4bytes from the target memory whose address is specified by `srcAddr'.
 * The value of the stored data is returned after swapping the endian.
 */
static sn_uint32 getMemoryU32(sn_uint32 processId, sn_uint64 threadId, 
                              sn_uint32 srcAddr) {
    sn_uint32 val;
    unsigned char buffer[4];
    sn_address snaddr;

    snaddr.word[0] = srcAddr;
    snaddr.word[1] = 0;

    if (SN_FAILED(PS3GetMemory(processId, threadId, buffer, snaddr, 4))) {
        printf(GetLastErrorString());
    }
    memcpy(&val, buffer, 4);

    return EndianSwap_4(val);
}


unsigned int IsValidFwdOrBk( sn_uint32 checkAddress )
{
    //NULL pointer is clearly bad
    if ( checkAddress == 0x00000000 )
        return 0;

    //Check for at least 4 byte alignment
    //(Should I be checking for 8 byte alignment?)
    if ( (checkAddress&0x3) != 0 )
        return 0;

    return 1;
}


/**
 * Dump out the basic information and the all chunks in the segment which is 
 * pointed by `segAddr'.
 **/
static int dumpSegment(FILE *fp, sn_uint32 processId,
                       sn_uint64 threadId, sn_uint32 segAddr) {
    sn_uint32 segBase;
    sn_uint32 segSize;
    sn_uint32 chunkPointer;
    sn_uint32 chunkCounter;
    sn_uint32 size;

    /* Get the base address and size of the target segment */
    segBase = getMemoryU32(processId, threadId, segAddr);
    segSize = getMemoryU32(processId, threadId, (segAddr + 4));

    if (fp != NULL) {
        fprintf(fp, "\tbase address: 0x%08x\n", segBase);
        fprintf(fp, "\tsize        : 0x%08x\n", segSize);
        fprintf(fp, "\t== chunks\n");
    } else {
        printf("\tbase address: 0x%08x\n", segBase);
        printf("\tsize        : 0x%08x\n", segSize);
        printf("\t== chunks\n");
    }
    /* the first chunk in the target segment has 8 bytes offset (padding) */
    chunkPointer = (segBase + 8);

    /* Iterate all over the chunks in the target segment */
    for (chunkCounter = 0; ; chunkCounter++) {
        /* Get the information about a chunk */
        //sn_uint32 prevChunkSize = getMemoryU32(processId, threadId, chunkPointer);
        sn_uint32 currentChunkSize = getMemoryU32(processId, threadId,
                                                  (chunkPointer + 4));
        sn_uint32 bIsInUse = (currentChunkSize & 0x2);

        if (fp != NULL) {
            if ( bIsInUse )
            {
                fprintf(fp, "\t[%06d] pos = 0x%08x, size = 0x%08x, [%s]\n",
                        chunkCounter, chunkPointer, (currentChunkSize & ~0x3),
                        "inuse");
            }
            else
            {
                sn_uint32 fwd   = getMemoryU32(processId, threadId,
                                               (chunkPointer + 8));
                sn_uint32 bk    = getMemoryU32(processId, threadId,
                                               (chunkPointer + 0xC));
                fprintf(fp, "\t[%06d] pos = 0x%08x, size = 0x%08x, [fwd=0x%08X, bk=0x%08X]\n",
                    chunkCounter, chunkPointer, (currentChunkSize & ~0x3),
                    fwd, bk);
                if ( !IsValidFwdOrBk( fwd ) )
                {
                    fprintf(fp, "\t\tBUG: fwd is corrupt\n");
                }
                if ( !IsValidFwdOrBk( bk ) )
                {
                    fprintf(fp, "\t\tBUG: bk is corrupt\n");
                }
            }
        } else {
            if ( bIsInUse )
            {
                printf("\t[%06d] pos = 0x%08x, size = 0x%08x, [%s]\n",
                       chunkCounter, chunkPointer, (currentChunkSize & ~0x3),
                       "inuse");
            }
            else
            {
                sn_uint32 fwd   = getMemoryU32(processId, threadId,
                                               (chunkPointer + 8));
                sn_uint32 bk    = getMemoryU32(processId, threadId,
                                               (chunkPointer + 0xC));
                printf("\t[%06d] pos = 0x%08x, size = 0x%08x, [fwd=0x%08X, bk=0x%08X]\n",
                    chunkCounter, chunkPointer, (currentChunkSize & ~0x3),
                    fwd, bk);
                if ( !IsValidFwdOrBk( fwd ) )
                {
                    printf("\t\tBUG: fwd is corrupt\n");
                }
                if ( !IsValidFwdOrBk( bk ) )
                {
                    printf("\t\tBUG: bk is corrupt\n");
                }
            }
        }

        if (currentChunkSize == 0x7) break;

        /* Get the top address of the next chunk */
        size = (currentChunkSize & ~0x3);
        chunkPointer += size;
        if (chunkPointer >= (segBase + segSize)) break;

        /* collect summary */
        if (!(currentChunkSize & 0x2)) {
            sFreedSize += size;
            if (sMaxFreedSize < size)
                sMaxFreedSize = size;
        } else {
            sInuseSize += size;
            if (sMaxInuseSize < size)
                sMaxInuseSize = size;
        }
    }

    return 0;
}


/**
 * Iterate all over the heap area and dump the information out.
 **/
static int dumpHeapArea(FILE *fp, sn_uint32 processId, sn_uint64 threadId) {
    sn_uint32 gmAddr;
    sn_uint32 segAddr;
    sn_val result;
    sn_uint32 magic;
    sn_uint32 segCounter;

    /* Get the address of the global heap management information */
    if (SN_FAILED(PS3EvaluateExpression(processId, threadId, 
                                        &result, "_gm_"))) {
        printf(GetLastErrorString());
    }

    /* If the debugger get a pointer to _gm_.
       Note that there are two _gm_s, one is a structure of a malloc
       management area, the other one is a pointer to the structure. */
    if (result.type == snval_address) {
        result.val.u32 = getMemoryU32(processId, threadId, 
                                      result.val.Address.word[0]);
        result.type = snval_uint32;
    }
    gmAddr = result.val.u32;

    /* confirmation _gm_ by using magic number */
    magic = getMemoryU32(processId, threadId, (gmAddr + MAGIC_OFFSET));
    if (magic != MAGIC_NUMBER) {
        printf("\n\ncould not find a management area of malloc, exiting...\n");
        return 1;
    }

    /* ok, magic is correct! */
    /* next, get the address of the segment structure */
    segAddr = gmAddr + SEG_OFFSET;

    /* iterate all over the segment */
    for (segCounter = 0; segAddr != 0; segCounter++) {
        if (fp != NULL) {
            fprintf(fp, "\n----\nSegment[%04d]:\n", segCounter);
        } else {
            printf("\n----\nSegment[%04d]:\n", segCounter);
        }
        dumpSegment(fp, processId, threadId, segAddr);

        segAddr = getMemoryU32(processId, threadId, 
                               (segAddr + NEXT_SEG_OFFSET));
    }

    /* Print summary */
    if (fp != NULL) {
        fprintf(fp, "\n\n----------\n");
        fprintf(fp, "inuse = %9u (max continuous: %u)\n", 
                sInuseSize, sMaxInuseSize);
        fprintf(fp, "freed = %9u (max continuous: %u)\n", 
                sFreedSize, sMaxFreedSize);
        fprintf(fp, "total = %9u\n", (sInuseSize + sFreedSize));

        printf("\n  -- SUMMARY --\n");
        printf("  inuse = %9u (max continuous: %u)\n", 
               sInuseSize, sMaxInuseSize);
        printf("  freed = %9u (max continuous: %u)\n", 
               sFreedSize, sMaxFreedSize);
        printf("  total = %9u\n", (sInuseSize + sFreedSize));
    } else {
        printf("\n\n----------\n");
        printf("inuse = %9u (max continuous: %u)\n", 
               sInuseSize, sMaxInuseSize);
        printf("freed = %9u (max continuous: %u)\n", 
               sFreedSize, sMaxFreedSize);
        printf("total = %9u\n", (sInuseSize + sFreedSize));
    }

    return 0;
}


/**
 * Add a menu item "Dump Heap Area" in `Source View'.
 * When the item is selected, the information about the heap area
 * which is managed by the default malloc in the standard library
 * is printed out into the `Script View'.
 *
 * The information is dumped out into a file which is selected from dialogue.
 * If you cancel to select a file, the standard output is used instead.
 **/
int main(int argc, char **argv) {
    sn_uint32 processId;
    sn_uint64 threadId;
    const sn_uint32 size32 = 4;
    const sn_uint32 size64 = 8;
    sn_notify notify;
    sn_uint32 itemId[10];

    TextClear();

    AddMenuItem("Dump Heap Area", &itemId[0], "DBGSourceView");
    RequestNotification(NT_CUSTMENU);

    while (1) {
        GetNotification(NT_CUSTMENU, &notify);
        if ((sn_uint32)notify.pParam1 == itemId[0]) {
            FILE *fp;
            GetObjectData(LOWORD(notify.pParam2), GVD_THREAD, 
                          &threadId, size64);
            GetObjectData(LOWORD(notify.pParam2), GVD_PROCESS, 
                          &processId, size32);

            /* initialize global objects for summary */
            sInuseSize = 0;
            sFreedSize = 0;
            sMaxInuseSize = 0;
            sMaxFreedSize = 0;

            printf("== DUMP BEGIN ==\n");
            FileSelectionDialog(filePath, fileName);
            fp = fopen(filePath, "w");
            if (NULL == fp) {
                printf("can not open file, use stdout\n");
            }

            if (fp != NULL)
                printf("  output file: %s\n", filePath);

            dumpHeapArea(fp, processId, threadId);

            if (fp != NULL)
                fclose(fp);

            printf("\n== DUMP END ==\n\n");
            break;
        }
    }


    return 0;
}

