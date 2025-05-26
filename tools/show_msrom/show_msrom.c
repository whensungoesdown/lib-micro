#define _GNU_SOURCE

#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "ldat.h"
#include "patch.h"

#include "ucode_macro.h"

void ms_ro_code_dump(void);
void ms_ro_seqw_dump(void);

int main(int argc, char* argv[]) 
{
    int i = 0;

    assign_to_core(0);

    ms_ro_code_dump();

    printf("\n\n");
    
    ms_ro_seqw_dump();

    return 0;
}
