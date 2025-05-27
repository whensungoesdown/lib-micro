#define _GNU_SOURCE

#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "ldat.h"
#include "patch.h"

#include "ucode_macro.h"


#define JUMP_DESTINATION 0x7dbc

void install_jump_target(void) {
    unsigned long addr = JUMP_DESTINATION;

    ucode_t ucode_patch[] = {
    //U3ccc: 11890b8279c8                 rdi:= ADDSUB_DSZ16_CONDD(IMM_MACRO_ALIAS_DATASIZE, rdi)
    //U3ccd: 11890b826988                 rsi:= ADDSUB_DSZ16_CONDD(IMM_MACRO_ALIAS_DATASIZE, rsi)
    {0x11890b8279c8, 0x11890b8279c8, 0x11890b826988, NOP_SEQWORD},
    {0x11890b826988, NOP, NOP, NOP_SEQWORD}, 
    {SUB_DSZ32_DRR(RCX, RCX, RCX) | MOD1, GENARITHFLAGS_IR(0x0000003f, TMP10), SFENCE, END_SEQWORD} // SEQW UEND0

    };

    printf("patching addr: %08lx - ram: %08lx\n", addr, ucode_addr_to_patch_addr(addr));
    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
    //printf("jump_target return value: 0x%lx\n", ucode_invoke(addr));
    print_patch(addr, ucode_patch, ARRAY_SZ(ucode_patch));
}


void hook_cmps(u64 addr, u64 hook_address, u64 idx)
{
    if (hook_address % 4 != 0) {
        printf("persistent_trace only supports 4-aligned uaddrs currently. (%04lx)\n", hook_address);
        return;
    }

    install_jump_target();

    u64 uop0 = ldat_array_read(0x6a0, 0, 0, 0, hook_address+0) & CRC_UOP_MASK;
    u64 uop1 = ldat_array_read(0x6a0, 0, 0, 0, hook_address+1) & CRC_UOP_MASK;
    u64 uop2 = ldat_array_read(0x6a0, 0, 0, 0, hook_address+2) & CRC_UOP_MASK;
    u64 seqw = ldat_array_read(0x6a0, 1, 0, 0, hook_address)   & CRC_SEQ_MASK;
    printf("0x3cc8 uop 0x%llx\n", (long long unsigned int)uop0);
    printf("0x3cc8 uop 0x%llx\n", (long long unsigned int)uop1);
    printf("0x3cc8 uop 0x%llx\n", (long long unsigned int)uop2);
    printf("      seqw 0x%llx\n", (long long unsigned int)seqw);

    ucode_t ucode_patch[] = {
        {   // 0x0
            // 64-bit 0xa79016d797e6bd3d
            // 32-bit 0x97e6bd3d
            NOP,
            LDZX_DSZ32_ASZ32_SC1_DR(TMP1, RDI, 0x08) | MOD1,  // dst_reg, src_reg, seg 0x08 es, 0x18 ds
            ZEROEXT_DSZ32_DI(TMP0, 0xa790),
            NOP_SEQWORD
        },
        {   // 0x4
            SHL_DSZ64_DRI(TMP0, TMP0, 0x10),
            ADD_DSZ64_DRI(TMP0, TMP0, 0x16d7),
            SHL_DSZ64_DRI(TMP0, TMP0, 0x10),
            NOP_SEQWORD
        },
        {   // 0x8 
            ADD_DSZ64_DRI(TMP0, TMP0, 0x97e6),
            SHL_DSZ64_DRI(TMP0, TMP0, 0x10),
            ADD_DSZ64_DRI(TMP0, TMP0, 0xbd3d),
            NOP_SEQWORD
        },
        {   // 0xc
            NOP,
            SUB_DSZ32_DRR(TMP10, TMP1, TMP0) | MOD1,   // dst, src0, src1
            UJMPCC_DIRECT_NOTTAKEN_CONDZ_RI(TMP10, JUMP_DESTINATION),
            NOP_SEQWORD
                //0x018000e5, //SUB MSLOOP 
                // BUG FIX: no MSLOOP, msloop cause gdb traped at repe cmps with resume flag (RF) set
        },
        {   // 0x10
            uop0, uop1, uop2, seqw
        },
        {   // 0x1c
            UJMP_I(hook_address+4), UJMP_I(hook_address+5), UJMP_I(hook_address+6), NOP_SEQWORD
        }
    };

    printf("Patching %04lx -> %04lx\n", hook_address, addr);
    print_patch(addr, ucode_patch, ARRAY_SZ(ucode_patch));

    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
    hook_match_and_patch(idx, hook_address, addr);
}

//#define CMPS_XLAT   0x3cc8
#define CMPS_XLAT   0x3de8

int main(int argc, char* argv[])
{
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        printf("core %d\n", i);
        assign_to_core(i);
        do_fix_IN_patch();
        hook_cmps(0x7dc8, CMPS_XLAT, 30);
        sleep(1);
    }

    return 0;
}
