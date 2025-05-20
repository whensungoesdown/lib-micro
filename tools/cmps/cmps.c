#define _GNU_SOURCE

#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "ldat.h"
#include "patch.h"

#include "ucode_macro.h"

//u8 verbose = 0;

//#define JUMP_DESTINATION 0x7c10
#define JUMP_DESTINATION 0x7c00

void install_jump_target(void) {
    unsigned long addr = JUMP_DESTINATION;

    ucode_t ucode_patch[] = {
    //{ MOVE_DSZ64_DI(RAX, 0x1234), MOVE_DSZ64_DR(RBX, TMP1), NOP,
//    { MOVE_DSZ64_DI(RAX, 0x1234), NOP, NOP,
//            SEQ_NEXT | SEQ_SYNCWTMRK(2) }, //0x7d00
//    { MOVE_DSZ64_DI(RAX, 0x1234), NOP, NOP,
//    { NOP, NOP, NOP,
//            END_SEQWORD }, //0x7d00
    //{MOVE_DSZ64_DI(RAX, 0x1234), NOP, NOP, END_SEQWORD} // SEQW UEND0

    //{NOP, NOP, XOR_DSZ64_DRR(RCX, RCX, RCX), NOP_SEQWORD},
    //{SUBR_DSZ64_DRR(TMP10, TMP10, TMP10), GENARITHFLAGS_IR(0x0000003f, TMP10), SFENCE, 0x0b0000f2} // SEQW UEND0
    //{XOR_DSZ64_DRR(RCX, RCX, RCX), GENARITHFLAGS_IR(0x0000003f, TMP10), SFENCE, 0x0b0000f2} // SEQW UEND0
//    {SUB_DSZ32_DRR(RCX, RCX, RCX) | MOD1, 0x237d3f000e88, SFENCE, 0x0b0000f2} // SEQW UEND0
//    {SUB_DSZ32_DRR(RCX, RCX, RCX) | MOD1, 0x237d3f000e88, 0x0fff00000000, 0x0b0000f2} // SEQW UEND0
    {SUB_DSZ32_DRR(RCX, RCX, RCX), 0x237d3f000e88, 0x0fff00000000, 0x0b0000f2} // SEQW UEND0



//        {UNK256, NOP, NOP, END_SEQWORD}, //0x7d04
    };
//    if (verbose)
        printf("patching addr: %08lx - ram: %08lx\n", addr, ucode_addr_to_patch_addr(addr));
    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    if (verbose)
        printf("jump_target return value: 0x%lx\n", ucode_invoke(addr));
        print_patch(addr, ucode_patch, ARRAY_SZ(ucode_patch));
}


void hook_cmps(u64 addr, u64 hook_address, u64 idx) {
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
            //UJMP_I(addr+0x4),
            //UJMP_I(addr+0x15),
            //UJMP_I(addr+0x16),
// 64-bit 0xa79016d797e6bd3d
// 32-bit 0x97e6bd3d
            //NOP,
            //MOVEFROMCREG_DSZ64_DI(TMP10, 0x0),
            //0x21e3b000200, //SIGEVENT(0x0000003b)
            NOP,
            //0x1c0000231027, //
            LDZX_DSZ64_ASZ32_SC1_DR(TMP1, RDI, 0x08),  // dst_reg, src_reg, seg
//            LDZX_DSZ32_ASZ32_SC1_DR(TMP1, RDI, 0x08) | MOD1,  // dst_reg, src_reg, seg
            //LDZX_DSZ64_ASZ64_SC8_DR(TMP1, RDI, 0x08),  // _LDZX_DSZ64_ASZ64_SC8 not defined in include/opcode.h, for now, only use 32-bit 
            ZEROEXT_DSZ32_DI(TMP0, 0xa790),
//            ZEROEXT_DSZ64_DI(TMP0, 0xa790),
            NOP_SEQWORD
        },
        {   // 0x4
//            SHL_DSZ32_DRI(TMP0, TMP0, 0x10),
//            ADD_DSZ32_DRI(TMP0, TMP0, 0x16d7),
//            SHL_DSZ32_DRI(TMP0, TMP0, 0x10),
            SHL_DSZ64_DRI(TMP0, TMP0, 0x10),
            ADD_DSZ64_DRI(TMP0, TMP0, 0x16d7),
            SHL_DSZ64_DRI(TMP0, TMP0, 0x10),
            NOP_SEQWORD

        },
        {   // 0x8 
//            ADD_DSZ32_DRI(TMP0, TMP0, 0x97e6),
//            SHL_DSZ32_DRI(TMP0, TMP0, 0x10),
//            ADD_DSZ32_DRI(TMP0, TMP0, 0xbd3d),
            ADD_DSZ64_DRI(TMP0, TMP0, 0x97e6),
            SHL_DSZ64_DRI(TMP0, TMP0, 0x10),
            ADD_DSZ64_DRI(TMP0, TMP0, 0xbd3d),
            NOP_SEQWORD
        },
        {   // 0xc
            NOP,
            //SUBR_DSZ64_DRR(TMP0, TMP0, TMP1),   // dst, src0, src1
            //0x10050003ac31, 
            //SUBR_DSZ32_DRR(TMP10, TMP1, TMP0),   // dst, src0, src1
//            SUB_DSZ32_DRR(TMP10, TMP1, TMP0),   // dst, src0, src1
            SUB_DSZ32_DRR(TMP10, TMP1, TMP0),   // dst, src0, src1
	        UJMPCC_DIRECT_NOTTAKEN_CONDZ_RI(TMP10, JUMP_DESTINATION),
            //NOP_SEQWORD
            0x018000e5, //NOP_SEQWORD, SUB MSLOOP
        },
//        {   // 0x10
//            NOP,
//            NOP,
//            NOP_SEQWORD
//        },
        {   // 0x14  0x10
            uop0, uop1, uop2, seqw
        },
        {   // 0x18  0x1c
            UJMP_I(hook_address+4), UJMP_I(hook_address+5), UJMP_I(hook_address+6), NOP_SEQWORD
        }
    };

    //if (verbose) {
        printf("Patching %04lx -> %04lx\n", hook_address, addr);
        print_patch(addr, ucode_patch, ARRAY_SZ(ucode_patch));
    //}

    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
    hook_match_and_patch(idx, hook_address, addr);
}

int test(void)
{

    long long unsigned int data = 0x00000000000000AA;
    unsigned char a[] = "abcdefgh\0";
    unsigned char* b = (unsigned char*)&data;
    unsigned char c[] = "abcdefgh\0";

    int ret = 0;
    long long int retrax = 0xff;

    //        ret = memcmp(a, b, 8);

    asm ("cld\n\t"
            //"movq $1, %%rcx\n\t"
            "xor %%rcx, %%rcx\n\t"
            "inc %%rcx\n\t"
            "movq %1, %%rsi\n\t"
            "movq %2, %%rdi\n\t"
            "repe cmpsq\n\t"
            "movq %%rax, %0\n\t"
            //"jne notequal\n\t"
            : "=r" (retrax)
            : "r" (a), "r"(b)
            :
        );

    if (0 == ret)
    {
        printf("a == b\n");
    }
    else
    {
        asm("notequal:");
        printf("a != b\n");
    }


    printf("rax 0x%llx\n", retrax);

    return 0;

}

struct _match_n_patch
{
    unsigned int p:1;
    unsigned int src:15;
    unsigned int dst:15;
    unsigned int reserve:1;
};

int list_match_patch()
{
    int i = 0;

    int value = 0;

    struct _match_n_patch* p  = 0;

    //init_match_and_patch();

    printf("idx p src   dst\n");

    for (i = 0; i < 32; i++)
    {
        value = ms_match_n_patch_read(i);

        p = (struct _match_n_patch*)&value;

        printf("%02d: %d 0x%04x  0x%04x\n", i, p->p, p->src * 2, p->dst * 2);
    }


    return 0;
}

struct _seqw
{
    unsigned int up0:2;
    unsigned int eflow:4;
    unsigned int up1:2;
    unsigned int uaddr:15;
    unsigned int up2:2;
    unsigned int sync:3;
    unsigned int crc:2;
    unsigned int reserved:2;
};

void print_seqw (int value)
{
    struct _seqw* p = 0;

    p = (struct _seqw*)&value;

    printf("up0  : 0x%x\n", p->up0);
    printf("eflow: 0x%x\n", p->eflow);
    printf("up1  : 0x%x\n", p->up1);
    printf("uaddr: 0x%x\n", p->uaddr);
    printf("up2  : 0x%x\n", p->up2);
    printf("sync : 0x%x\n", p->sync);
    printf("crc  : 0x%x\n", p->crc);

    return;
}

int main(int argc, char* argv[]) {


    int i = 0;

    assign_to_core(0);

//    // test
//    printf("SEQ_UEND0(0) 0x%x\n", SEQ_UEND0(0));
//    printf("SEQ_UEND0(1) 0x%x\n", SEQ_UEND0(1));
//    printf("SEQ_UEND0(2) 0x%x\n", SEQ_UEND0(2));
//    printf("SEQ_UEND0(3) 0x%x\n", SEQ_UEND0(3));
//
//    printf("END_SEQWORD 0x%x\n", END_SEQWORD);
//

//    printf("0x018000f2\n");
//    print_seqw(0x018000f2);
//    printf("\n");
//
//    printf("0x018000c0\n");
//    print_seqw(0x018000c0);
//    printf("\n");
//
//    printf("NOP_SEQWORD\n");
//    print_seqw(NOP_SEQWORD);
//    printf("\n");
//
//    printf("END_SEQWORD\n");
//    print_seqw(END_SEQWORD);
//    printf("\n");
//
//    printf("0x80000c0\n");
//    print_seqw(0x8000c0);
//    printf("\n");
//
//    printf("0x18b9100\n");
//    print_seqw(0x18b9100);
//    printf("\n");
//
//    printf("0x018000e5\n");
//    print_seqw(0x018000e5);
//    printf("\n");

//    printf("LDZX_DSZ64_ASZ32_SC1_DR(TMP1, RDI, 0x08) : 0x%x\n", LDZX_DSZ64_ASZ32_SC1_DR(TMP1, RDI, 0x08));
//    printf("GENARITHFLAGS_IR(0x0000003f, TMP10) : 0x%x\n", GENARITHFLAGS_IR(0x0000003f, TMP10));
//    printf("SFENCE : 0x%x\n", SFENCE);
//    return 0;



//    do_fix_IN_patch();

//    usleep(20000);

//    init_match_and_patch();


//    usleep(20000);

//#define CMPS_XLAT   0x08b0
#define CMPS_XLAT   0x3cc8

    hook_cmps(0x7c10, CMPS_XLAT, 1);

    //    printf("press any key to run test().\n");
    //    getchar();

    //for (i = 0; i < 100; i++)
    {
//        test();
    }

//    usleep(20000);

//    list_match_patch();

    return 0;
}
