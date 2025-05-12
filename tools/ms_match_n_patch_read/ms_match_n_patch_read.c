#define _GNU_SOURCE

#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "ldat.h"
#include "patch.h"

#include "ucode_macro.h"

struct _match_n_patch
{
    unsigned int p:1;
    unsigned int src:15;
    unsigned int dst:15;
    unsigned int reserve:1;
};

int main(int argc, char* argv[]) 
{
    int i = 0;

    int value = 0;

    struct _match_n_patch* p  = 0;

    assign_to_core(0);

    //init_match_and_patch();
    //do_fix_IN_patch();

    printf("idx p src   dst\n");

    for (i = 0; i < 32; i++)
    {
        value = ms_match_n_patch_read(i);

        p = (struct _match_n_patch*)&value;

        printf("%02d: %d 0x%04x  0x%04x\n", i, p->p, p->src * 2, p->dst * 2);
    }


    return 0;
}
