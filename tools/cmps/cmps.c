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

#define JUMP_DESTINATION 0x7c10
void install_jump_target(void) {
    unsigned long addr = JUMP_DESTINATION;

    ucode_t ucode_patch[] = {
    //{ MOVE_DSZ64_DI(RAX, 0x1234), MOVE_DSZ64_DR(RBX, TMP1), NOP,
    //        SEQ_NEXT | SEQ_SYNCWTMRK(2) }, //0x7d00
    { MOVE_DSZ64_DI(RAX, 0x1234), NOP, NOP,
            END_SEQWORD }, //0x7d00
        {UNK256, NOP, NOP, END_SEQWORD}, //0x7d04
    };
//    if (verbose)
        printf("patching addr: %08lx - ram: %08lx\n", addr, ucode_addr_to_patch_addr(addr));
    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    if (verbose)
        printf("jump_target return value: 0x%lx\n", ucode_invoke(addr));
}

//void persistent_trace(u64 addr, u64 hook_address, u64 idx) {
//    if (hook_address % 4 != 0) {
//        printf("persistent_trace only supports 4-aligned uaddrs currently. (%04lx)\n", hook_address);
//        return;
//    }
//    install_jump_target();
//    u64 uop0 = ldat_array_read(0x6a0, 0, 0, 0, hook_address+0) & CRC_UOP_MASK;
//    u64 uop1 = ldat_array_read(0x6a0, 0, 0, 0, hook_address+1) & CRC_UOP_MASK;
//    u64 uop2 = ldat_array_read(0x6a0, 0, 0, 0, hook_address+2) & CRC_UOP_MASK;
//    u64 seqw = ldat_array_read(0x6a0, 1, 0, 0, hook_address)   & CRC_SEQ_MASK;
//
//    ucode_t ucode_patch[] = {
//        {   // 0x0
//            UJMP_I(addr+0x4),
//            UJMP_I(addr+0x15),
//            UJMP_I(addr+0x16),
//            NOP_SEQWORD
//        },
//        {   // 0x4
//            TESTUSTATE_SYS_NOT(0x2),
//            STADSTGBUF_DSZ64_ASZ16_SC1_RI(TMP0, 0xba40),
//            STADSTGBUF_DSZ64_ASZ16_SC1_RI(TMP1, 0xba80),
//            SEQ_GOTO0(addr+0x14)
//        },
//        {   // 0x8
//            ZEROEXT_DSZ32_DI(TMP0, 0xdead),
//            CONCAT_DSZ16_DRR(TMP0, TMP0, TMP0),
//            CONCAT_DSZ32_DRR(TMP0, TMP0, TMP0),
//            NOP_SEQWORD
//        },
//        {   // 0xc
//            XOR_DSZ64_DRR(TMP0, TMP0, RAX),
//            MOVE_DSZ64_DI(TMP1, hook_address),
//            UJMPCC_DIRECT_NOTTAKEN_CONDZ_RI(TMP0, JUMP_DESTINATION),
//            NOP_SEQWORD,
//        },
//        {   // 0x10
//            LDSTGBUF_DSZ64_ASZ16_SC1_DI(TMP0, 0xba40),
//            LDSTGBUF_DSZ64_ASZ16_SC1_DI(TMP1, 0xba80),
//            NOP,
//            NOP_SEQWORD,
//        },
//        {   // 0x14
//            uop0, uop1, uop2, seqw
//        },
//        {   // 0x18
//            UJMP_I(hook_address+4), UJMP_I(hook_address+5), UJMP_I(hook_address+6), NOP_SEQWORD
//        }
//    };
//
//    if (verbose) {
//        printf("Patching %04lx -> %04lx\n", hook_address, addr);
//        print_patch(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    }
//
//    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    hook_match_and_patch(idx, hook_address, addr);
//}
//
//u64 make_seqw_goto_syncfull(u64 target_addr) {
//    u64 seqw =  0x9000080 | ((target_addr & 0x7fff) << 8);
//    return seqw | (parity0(seqw) << 28) | (parity1(seqw) << 29);
//}
//void insert_trace(u64 tracing_addr) {
//    #include "ucode/trace.h"
//
//    patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    hook_match_and_patch(0, tracing_addr, addr);   
//}
//
//const char *argp_program_version = "custom-cpu v0.1";
//static char doc[] = "Tool for patching ucode";
//static char args_doc[] = "";
//
//// cli argument availble options.
//static struct argp_option options[] = {
//    {.name="verbose", .key='v', .arg=NULL, .flags=0, .doc="Produce verbose output"},
//    {.name="reset", .key='r', .arg=NULL, .flags=0, .doc="reset match & patch"},
//    {.name="wait_trace", .key='w', .arg=NULL, .flags=0, .doc="wait for hit on trace"},
//    {.name="xlat_fuzz", .key='x', .arg="uaddrs", .flags=0, .doc="start fuzzing xlats"},
//    {.name="trace", .key='t', .arg="uaddr", .flags=0, .doc="trace ucode addr"},
//    {.name="backtrace", .key='b', .arg="uaddr,reg", .flags=0, .doc="get previous ucode addr"},
//    {.name="core", .key='c', .arg="core", .flags=0, .doc="core to patch [0-3]"},
//    {0}
//};
//
//
//// define a struct to hold the arguments.
//struct arguments{
//    u8 verbose;
//    u8 reset;
//    u8 wait;
//    u8 xlat;
//    u8 bt_reg;
//    s32 bt_uaddr;
//    s32 trace_addr;
//    s32 uaddrs[0x10];
//    u8 uaddr_count;
//    s8 core;
//};
//
//
//// define a function which will parse the args.
//static error_t parse_opt(int key, char *arg, struct argp_state *state){
//    char *token;
//    int i;
//    struct arguments *arguments = state->input;
//    const char* uaddr_s;
//    const char* reg_s;
//    switch(key){
//
//        case 'v':
//            arguments->verbose = 1;
//            break;
//        case 'r':
//            arguments->reset = 1;
//            break;
//        case 'w':
//            arguments->wait = 1;
//            break;
//        case 'x':
//            token = strtok(arg, ",");
//            for (i = 0; i < 0x10 && token != NULL; i++, token = strtok(NULL, ",")) {
//                arguments->uaddrs[i] = strtol(token, NULL, 0);
//                if (arguments->uaddrs[i] < 0) {
//                    argp_usage(state);
//                    exit(EXIT_FAILURE);
//                }
//            }
//            arguments->uaddr_count = i;
//            arguments->xlat = 1;
//            break;
//        case 'b':
//            uaddr_s = strtok(arg, ",");
//            reg_s = strtok(NULL, "");
//            if (reg_s == NULL) {
//                argp_usage(state);
//                exit(EXIT_FAILURE);
//            }
//            arguments->bt_uaddr = strtol(uaddr_s, NULL, 0);
//            arguments->bt_reg = strtol(reg_s, NULL, 0);
//
//            if (arguments->bt_uaddr < 0 || arguments->bt_reg > 0xf) {
//                argp_usage(state);
//                exit(EXIT_FAILURE);
//            }
//            break;
//        case 't':
//            arguments->trace_addr = strtol(arg, NULL, 0);
//            if (arguments->trace_addr < 0){
//                argp_usage(state);
//                exit(EXIT_FAILURE);
//            }
//            break;
//        case 'c':
//            arguments->core = strtol(arg, NULL, 0);
//            if (arguments->core < 0 || arguments->core > 3){
//                argp_usage(state);
//                exit(EXIT_FAILURE);
//            }
//            break;
//
//        default:
//            return ARGP_ERR_UNKNOWN;
//    }
//
//    return 0;
//}
//
//general_purpose_regs try_xlat(u64 val) {
//    u64 rax = val;
//    general_purpose_regs result = {0};
//    lmfence();
//    asm volatile(
//        "sysenter\n\t"
//        : "=a" (result.rax)
//        , "=b" (result.rbx)
//        , "=c" (result.rcx)
//        , "=d" (result.rdx)
//        : "a" (rax), "b"(&result.rbx), "c"(3), "d"(4)
//        : "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
//    );
//    lmfence();
//    return result;
//}
//
//void xlat_fuzzing(int* uaddrs, int size) {
//
//    staging_write(0xba00, 0x0);
//    uram_write(0x2c, 0x0);
//
//    u64 cpuid_xlat = 0x0be0;
//    u64 pause_xlat = 0x0bf0;
//    u64 iret_xlat = 0x07c8; //think this is wrong
//    u64 vmxon_xlat = 0x0ae8;
//    u64 vmxoff_xlat = 0x08c8;
//    u64 vmlaunch_xlat = 0x0328;
//    u64 vmclear_xlat = 0x0af8;
//    u64 hlt_xlat = 0x0818;
//
//    u64 uaddr = 0x7c20;
//    for (u64 i = 0; i < size; i++) {
//        persistent_trace(uaddr + i * 0x20, uaddrs[i], i);
//    }
//    general_purpose_regs val = try_xlat(0xdeaddeaddeaddeadUL);
//    printf("rax:        0x%lx\n", val.rax);
//    printf("rbx:        0x%lx\n", val.rbx);
//    printf("rcx:        0x%lx\n", val.rcx);
//    printf("rdx:        0x%lx\n", val.rdx);
//    puts("");
//}
//
//void do_backtrace(u64 hook_address, u64 testreg) {
//    u64 aligned_hook_address = hook_address & ~3;
//    u64 uop0 = ldat_array_read(0x6a0, 0, 0, 0, aligned_hook_address+0) & CRC_UOP_MASK;
//    u64 uop1 = ldat_array_read(0x6a0, 0, 0, 0, aligned_hook_address+1) & CRC_UOP_MASK;
//    u64 uop2 = ldat_array_read(0x6a0, 0, 0, 0, aligned_hook_address+2) & CRC_UOP_MASK;
//    u64 seqw = ldat_array_read(0x6a0, 1, 0, 0, aligned_hook_address)   & CRC_SEQ_MASK;
//
//    unsigned long addr = 0x7c10;
//    if (hook_address % 4 == 0) {
//        #include "ucode/backtrace0.h"
//        patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    } else if (hook_address % 4 == 1) {
//        #include "ucode/backtrace1.h"
//        patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    } else if (hook_address % 4 == 2) {
//        #include "ucode/backtrace2.h"
//        patch_ucode(addr, ucode_patch, ARRAY_SZ(ucode_patch));
//    } else {
//        printf("Invalid uaddr\n");
//        exit(-1);
//    }
//    if (verbose)
//        printf("Patching %04lx -> %04lx\n", hook_address & ~1, addr);
//    hook_match_and_patch(1, hook_address & ~1, addr);
//
//    general_purpose_regs val = try_xlat(0xdeaddeaddeaddeadUL);
//    printf("rax:        0x%lx\n", val.rax);
//    printf("rbx:        0x%lx\n", val.rbx);
//    printf("rcx:        0x%lx\n", val.rcx);
//    printf("rdx:        0x%lx\n", val.rdx);
//    puts("");
//}

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
    printf("0x08b0 uop 0x%llx\n", (long long unsigned int)uop0);
    printf("0x08b1 uop 0x%llx\n", (long long unsigned int)uop1);
    printf("0x08b2 uop 0x%llx\n", (long long unsigned int)uop2);
    printf("      seqw 0x%llx\n", (long long unsigned int)seqw);

    ucode_t ucode_patch[] = {
        {   // 0x0
            UJMP_I(addr+0x4),
            UJMP_I(addr+0x15),
            UJMP_I(addr+0x16),
            NOP_SEQWORD
        },
        {   // 0x4
            //TESTUSTATE_SYS_NOT(0x2),
            //STADSTGBUF_DSZ64_ASZ16_SC1_RI(TMP0, 0xba40),
            //STADSTGBUF_DSZ64_ASZ16_SC1_RI(TMP1, 0xba80),
            //SEQ_GOTO0(addr+0x14)

            LDZX_DSZ64_ASZ32_SC1_DR(TMP0, RDI, 0x18),  // dst_reg, src_reg, seg
            ZEROEXT_DSZ32_DI(TMP1, 0x4141),
            SUBR_DSZ64_DRR(TMP0, TMP0, TMP1),   // dst, src0, src1
	    //NOP,
	    //NOP,
	    //NOP,
            NOP_SEQWORD

        },
        {   // 0x8
            //ZEROEXT_DSZ32_DI(TMP0, 0xdead),
            //CONCAT_DSZ16_DRR(TMP0, TMP0, TMP0),
            //CONCAT_DSZ32_DRR(TMP0, TMP0, TMP0),
            //NOP_SEQWORD
            
	    UJMPCC_DIRECT_NOTTAKEN_CONDZ_RI(TMP0, JUMP_DESTINATION),
	    //MOVE_DSZ64_DI(RAX, 0xabcd),
        //    NOP,
            NOP,
            NOP,
            NOP_SEQWORD
	    //END_SEQWORD
        },
        {   // 0xc
            //XOR_DSZ64_DRR(TMP0, TMP0, RAX),
            //MOVE_DSZ64_DI(TMP1, hook_address),
            //UJMPCC_DIRECT_NOTTAKEN_CONDZ_RI(TMP0, JUMP_DESTINATION),
            //NOP_SEQWORD,
            NOP,
            NOP,
            NOP,
            NOP_SEQWORD
        },
        {   // 0x10
            //LDSTGBUF_DSZ64_ASZ16_SC1_DI(TMP0, 0xba40),
            //LDSTGBUF_DSZ64_ASZ16_SC1_DI(TMP1, 0xba80),
            //NOP,
            //NOP_SEQWORD,
            NOP,
            NOP,
            NOP,
            NOP_SEQWORD
        },
        {   // 0x14
            uop0, uop1, uop2, seqw
        },
        {   // 0x18
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

int main(int argc, char* argv[]) {


    int i = 0;

    assign_to_core(0);


//    do_fix_IN_patch();

//    usleep(20000);

//    init_match_and_patch();


//    usleep(20000);

#define CMPS_XLAT   0x08b0

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
