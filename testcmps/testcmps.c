#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>


__attribute__((always_inline))
void static inline assign_to_core(int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset) != 0){
        error(EXIT_FAILURE, -1, "assign to specific core failed.");
    }
}

int test(void)
{

        long long unsigned int data = 0xa79016d797e6bd3d;
        //long long unsigned int data = 0x0000000097e6bd3d;
        //long long unsigned int data = 0x00000000a79016d7;
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
             "jne notequal\n\t"
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


        return 0;
}

int main(void)
{
	assign_to_core(0);
	test();
//	assign_to_core(1);
//	test();
//	assign_to_core(2);
//	test();
//	assign_to_core(3);
//	test();
	return 0;
}
