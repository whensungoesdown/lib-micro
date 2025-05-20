#define _GNU_SOURCE

#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "ldat.h"
#include "patch.h"

#include "ucode_macro.h"

int main(int argc, char* argv[]) 
{
    int i = 0;

    assign_to_core(0);

    printf("ms ram:\n");

    for (i = 0x7c00; i < 0x7e00; i+=4)
    {
        u64 uop0 = ldat_array_read(0x6a0, 0, 0, 0, i+0) & CRC_UOP_MASK;
        u64 uop1 = ldat_array_read(0x6a0, 0, 0, 0, i+1) & CRC_UOP_MASK;
        u64 uop2 = ldat_array_read(0x6a0, 0, 0, 0, i+2) & CRC_UOP_MASK;
        u64 seqw = ldat_array_read(0x6a0, 1, 0, 0, i)   & CRC_SEQ_MASK;

        printf("%04x: %012lx %012lx %012lx %08lx\n", i, uop0, uop1, uop2, seqw);
    }


    return 0;
}
