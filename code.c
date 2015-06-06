#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/unistd.h> 
#include <linux/time.h> 
#include <asm/uaccess.h> 
#include <linux/sched.h>
//系统中未使用的系统调用号 unsigned int clear_and_return_cr0(void);
#define __NR_syscall 223
#define SYS_CALL_TABLE_ADDRESS 0xc057e110 //步骤 2 中的 sys_call_table 的地址
void setback_cr0(unsigned int val);
int orig_cr0;
unsigned long *sys_call_table = 0;
static int (*anything_saved)(void);
unsigned int clear_and_return_cr0(void) {
    unsigned int cr0 = 0; unsigned int ret;
    asm volatile ("movl %%cr0, %%eax" : "=a"(cr0)
                  ); ret = cr0;
    cr0 &= 0xfffeffff;
    asm volatile ("movl %%eax, %%cr0"
                  :
                  : "a"(cr0) );
    return ret;
}