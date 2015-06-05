Linux 内核模块和系统调用
1.实验目的:
a.了解 Linux 内核模块和系统调用的相关知识 b.通过实验了解内核模块和系统调用实验的机制
2.Linux 内核模块 2.1.微内核 VS 单内核
微内核:在微内核中,大部分内核都作为单独的进程在特权状态下运行,他 们通过消息传递进行通讯。在典型情况下,每个概念模块都有一个进程。因此, 假如在设计中有一个系统调用模块,那么就必然有一个相应的进程来接收系统调 用,并和能够执行系统调用的其他进程(或模块)通讯以完成所需任务。 在这些 设计中,微内核部分经常只是个消息转发站:当系统调用模块要给文档系统模块 发送消息时,消息直接通过内核转发。这种方式有助于实现模块间的隔离。
优点:
a. 无需的模块不会被加载到内存中,因此微内核就能够更有效的利用内存。
b. 充分的模块化,可独立更换任意模块而不会影响其他模块,从而方便于
第三方开发、设计模块
c. 具有很高的可移植性,理论上讲只需要单独对微内核各部分进行移植修
改即可。由于微内核的体积通常很小,而且互不影响,因此工作量很小。
缺点:效率问题
单内核:单内核是个很大的进程。它的内部又能够被分为若干模块(或是层 次或其他)。但是在运行的时候,他是个单独的二进制大映像。其模块间的通讯 是通过直接调用其他模块中的函数实现的,而不是消息传递。
优点:单内核模型以提高系统执行效率为设计理念,因为整个系统是一个
统一的内核,所以其内部调用效率很高。
缺点:通常各模块之间的界限并不特别清晰,模块间的调用比较随意,所 以进行系统修改或升级时,往往“牵一发而动全身”,导致工作量加大,使其难 于维护。
微内核与单内核之争由来已久,1992 年两位上古巨牛 Tanenbaum 与 Linus 就 因此事展开了一场论述;http://www.zhihu.com/question/20479578
2.2 Linux 如何对待这个问题
Linux 是一个单内核,也就是说,Linux 内核运行在单独的内核地址空间。不
过,Linux 汲取了微内核的精华:其引以为傲的是模块化设计、抢占式内核、支 持内核线程以及动态装载内核模块的能力。不仅如此,Linux 还避免微内核设计 上性能损失的缺陷,让所有事情都运行在内核态,直接调用函数,无需消息传递。 至今,Linux 是模块化的、多线程的以及内核本身可调度的操作系统。
2.3 LKM (Loadable Kernel Module) 内核模块是一种没有经过连接,不能独立运行的目标文件,在内核空间运
行的程序。经过链接装载到内核里面成为内核的一部分,可以访问内核的公用符 号;LKM 可以在运行时加载内核模块功能,在不需要时可以卸载相关内核模块 节约系统资源;
优点:模块本身不编译进内核,从而控制了内核的大小;模块一旦被加载, 将和其它的部分完全一样。
缺点:可能会有内核与模块版本不兼容的问题,导致内核崩溃;
￼
2.4 Linux 的内核模块相关命令 在内核模块开发的过程中常用的有以下指令。
1) insmod: 将模块插入内核中,使用方法:#insmod XXX.ko
2) rmmod: 将模块从内核中删除,使用方法:#rmmod XXX.ko
3) lsmod: 列表显示所有的内核模块,可以和 grep 指令结合使用。使用方
法:#lsmod | grep XXX
4) modprobe: modprobe 可载入指定的个别模块,或是载入一组相依赖的
模块。modprobe 会根据 depmod 所产生的依赖关系,决定要载入哪些模块。若在 载入过程中发生错误,在 modprobe 会卸载整组的模块。依赖关系是通过读取 /lib/modules/2.6.xx/modules.dep 得到的。而该文件是通过 depmod 所建立。
5) modinfo: 查看模块信息。使用方法:#modinfo XXX.ko 6) tree –a: 查看当前目录的整个树结构。使用方法:#tree -a
2.5 内核模块程序结构
￼#include <linux/init.h> //for moudle_init #include <linux/module.h> //must be include #include <linux/kernel.h> //for printk()
static int hello_init(void) {
    printk("hello word\n");
    return 0; }
static void hello_exit(void) {
    printk("hello module exit\n"); }
moudle_init(hello_init); module_exit(hello_exit);
MODULE_LICENSE("GPL"); MODULE_AUTHOR("froge");
1) 模块加载函数:来自模块初始化函数的非 0 返回值意味着模块执行失败 2) 模块卸载函数
3) 模块许可证声明:如果不声明,则在模块加载时会收到内核被污染的警告, 一般应遵循 GPL 协议
4) 模块信息
￼__ 2.5 内核模块参数传递
￼#include <linux/init.h>
#include <linux/module.h>
static char *bookName = "Good Book."; static int bookNumber = 100;
static int __init book_init(void) {
    printk(KERN_INFO "Book name is %s\n", bookName); printk(KERN_INFO "Book number is %d\n", bookNumber); return 0;
}
static void __exit book_exit(void) {
    printk(KERN_INFO "Book module exit.\n");
}
module_init(book_init); module_exit(book_exit); module_param(bookName, charp, S_IRUGO); module_param(bookNumber, int, S_IRUGO); MODULE_LICENSE("GPL");
模块在被加载时传递给模块的值,本身应该是模块内部的全局变量;在向内核 插入模块的时候可以用以下方式,并且可以在内核日志中看到模块加载以后变量 已经有了值。
￼￼
2.5 内核模块 Makefile Demo
￼obj-m += hello.o
#generate the path
CURRENT_PATH:=$(shell pwd)
#the current kernel version number
LINUX_KERNEL:=$(shell uname -r)
#the absolute path LINUX_KERNEL_PATH:=/usr/src/linux-headers-$(LINUX_KERNEL) #complie object
all:
make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules #clean
clean:
make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
3. Linux 系统调用
系统调用是操作系统提供给软件开发人员的唯一接口,开发人员可利用它 使用系统功能。OS 核心中都有一组实现系统功能的过程(子程序),系统调用 就是对上述过程的调用。因此,系统调用像一个黑箱子那样,对用户屏蔽了操 作系统的具体动作而只提供有关的功能。
系统调用在 os 中发挥着巨大的作用,如果没有系统调用那么应用程序就 是失去了内核的支持。在系统中真正被所有进程都使用的内核通信方式是系统 调用。例如当进程请求内核服务时,就使用的是系统调用。一般情况下,进程 是不能够存取系统内核的。它不能存取内核使用的内存段,也不能调用内核函 数,CPU 的硬件结构保证了这一点。只有系统调用是一个例外。进程使用寄存 器中适当的值跳转到内核中事先定义好的代码中执行,(当然,这些代码是只 读的)。在 Intel 结构的计算机中,这是由中断 0x80 实现的。 进程可以跳转 到的内核中的位置叫做 system_call。在此位置的过程检查系统调用号,它将告 诉内核进程请求的服务是什么。然后,它再查找系统调用表 sys_call_table,找到 希望调用的内核函数的地址,并调用此函数,最后返回。
所以,如果希望改变一个系统调用的函数,需要做的是编写一个自己的 函数,然后改变 sys_call_table 中的指针指向该函数,最后再使用 cleanup_module 将系统调用表恢复到原来的状态。我们现在向内核中添加的三个系统调用就是 属于向内核中添加新的函数,且这些函数是可以直接操作系统内核的。下面是 系统调用的基本处理过程:
￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼
￼3.1 Linux 系统调用机制:
在 Linux 系统中,系统调用是作为一种异常类型实现的。它将执行相应的机 器代码指令来产生异常信号。产生中断或异常的重要效果是系统自动将用户态切 换为核心态来对它进行处理。这就是说,执行系统调用异常指令时,自动地将系 统切换为核心态,并安排异常处理程序的执行。Linux 用来实现系统调用异常的 实际指令是:
Int $0x80
这一指令使用中断/异常向量号 128(即 16 进制的 80)将控制权转移给内核。
为达到在使用系统调用时不必用机器指令编程,在标准的 C 语言库中为每一系 统调用提供了一段短的子程序,完成机器代码的编程工作。事实上,机器代码段 非常简短。它所要做的工作只是将送给系统调用的参数加载到 CPU 寄存器中, 接着执行 int $0x80 指令。然后运行系统调用,系统调用的返回值将送入 CPU 的 一个寄存器中,标准的库子程序取得这一返回值,并将它送回用户程序。
整个过程:
1. 执行用户程序
2. 根据 glibc(GNU 实现的一套标准 C 的库函数)中的函数实现,取得系
￼￼￼￼￼￼￼￼￼￼￼￼￼￼
￼统调用号并执行 int $0x80 产生中断;
3. 进行地址空间的转换和堆栈的切换,执行 SAVE_ALL。(进入内核模式)
4. 进行中断处理,根据系统调用表调用内核函数;
5. 执行内核函数;
6. 执行 RESTORE_ALL 并返回用户模式;
3.2 添加系统调用 两种方法:
1. 修改内核文件,重新编译内核; 2. 采用内核模块的方式;
3.3 修改内核
￼￼￼￼￼￼￼￼For
1.
Ubuntu 9.04:
解压 linux-kernel-2.6.32.66 到 /usr/src/ 安装 xz 文件包:
tar -zxvf xz-5.2.1-tar.bz2
cd xz-5.2.1-tar.bz2
./configure
make
Sudo make install
Sudo ldconfig
解压 linux 源码文件
xz -d linux-2.6.32.66.xz
￼￼￼￼￼￼￼￼￼￼
￼sudo tar -xvf linux-2.6.32.66.tar -C /usr/src/
2. 系统调用函数实现:修改 kernel/sys.c 文件
￼￼asmlinkage int sys_mycall(int number) { printk("这是我添加的第一个系统调用");
    ￼return number; }
￼3. 设置系统调用号:
在/usr/src/linux-source/arch/x86/kernel/syscall_table_32.S 中添加:
.long sys_mycall
在 /usr/src/linux-source/arch/x86/include/asm/unistd_32.h 中 添 加 : #define __NR_mycall 序号(例如 337),添加系统调用的入口参数(注意:其中会顺 序定义入口参数的序号,添加的序号是在原有最大值的基础上+1);
4. 编译内核:
编译内核,命令依次如下:
首先切换到解压的内核目录下。
第一步:make mrproper //清除内核中不稳定的目标文件,附属文件及内核
配置文件
第二步:make clean //清除以前生成的目标文件和其他文件 第三步:sudomakeoldconfig// 采用默认的内核配置(使用makemenuconfig
可以自己配置编译选项)
第四步:sudo make bzImage //编译内核
￼￼￼￼￼￼￼￼￼￼￼￼￼￼
￼第五步:make modules //编译模块 第六步:make modules_install// 安装模块 整个完成大约需要两个小时!!!
5. 使用新内核
6. 设置启动项
￼￼￼￼￼cp /usr/src/linux-source-2.6.32/arch/boot/bzImage /boot/vmlinuz-2.6.32.66
1.
2. mkinitramfs -o initrd.img-2.6.32.66 2.6.32.66
￼3.
cp /usr/src/linux-source-2.6.32.66/initrd.img-2.6.32.66 /boot/initrd.img-2.6.32.66
￼￼￼￼￼7.
增加引导菜单项,配置启动项文件/boot/grub/menu.lst 参考里面的内容增加
新的启动项目 sudo update-grub; sudo reboot
￼测试系统调用:
￼重启后运行:uname –r 查看系统内核版本
￼￼8.
编写测试程序
￼￼￼For Ubuntu14.04
1. 解压 linux-kernel-3.18.14 到 /usr/src/
安装 xz 文件包:
tar -zxvf xz-5.2.1-tar.bz2 cd xz-5.2.1-tar.bz2 ./configure
make
￼￼￼￼￼￼
￼Sudo make install
Sudo ldconfig
解压 linux 源码文件
xz -d linux-3.18.14.xz
sudo tar -xvf linux-3.18.14.tar -C /usr/src/
2. 系统调用函数实现:修改 kernel/sys.c 文件
OR
3. 设置系统调用号:修改 arch/x86/syscalls/syscall_32.tb:
4. 添加系统调用声明到头文件:include/linux/syscalls.h
5.编译内核
首先安装 ncurses 库,这个库会在之后的编译中用到 apt-get install libncurses5-dev
vim mycall.sh 在 mycall.sh 写入一下内容
￼￼￼￼￼￼asmlinkage int sys_mycall( int number){
    printk("this is my first system call function \n"); return number;
}
SYSCALL_DEFINE1(mycall, int, number){
    printk("this is my first system call function \n"); return number;
} //DEFINEn 中的 n 表示参数个数. 此函数目的为返回输入值
￼￼￼￼￼￼￼￼￼￼加入 358
￼i386 mycall sys_mycall //358 即为新的系统调用号
￼￼￼asmlinkage int sys_mycall(int number);
￼OR
￼￼asmlinkage long sys_mycall(int number);
￼￼￼￼￼￼#!/bin/bash
make mrproper //清除旧的编译内容
￼￼￼make menuconfig //编译新内核的配置文件
￼￼make bzImage -j4 //生成新的内核镜像 make modules -j4 //编译模块
make modules_install –j4 //安装模块 make install -j4 //安装内核 update-grub //更新引导文件
￼￼￼￼￼reboot //重新启动
chmod 777 mycall.sh
./mycall.h :(提示:编译过程中,将出现内核配置文件修改窗口,选择<save>, 之后<exit>即可。即使用原先的配置,并不做修改。)
6.测试系统调用:
}
运行:
gcc main.c -o out
./a.out
Sudo dmesg -c
查看内核输出结果:
dmesg or tail -n 2 /var/log/kern.log 3.4 采用内核模块方式实现系统调用
原理:修改映射在内存中的系统调用表,把一个空闲的系统调用表项指向自己 写的模块中的函数
步骤:
1. 在 asm/unistd.h 查找 unsed 系统调用号
￼#include<stdio.h>
#include<unistd.h>
void main(){
    printf("%d\n", syscall(351, 1)) //351为系统调用号
    ￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼
    ￼Cat arch/x86/include/asm/unistd_32.h | grep “unused”
    223 unused
    2. 找系统调用表在内存中的位置
    Cat /boot/System.map-xxx.xx.xx|grep “sys_call_table”
    0xc0509940 R sys_call_table 其中 R 代表是只读的
    3. 为了修改内存中的表项,还要修改寄存器写保护位,否则是不能修改的。
    它位于 cr0 的 16 位,其修改和保存由模块初始化函数实现
    在 intel 手册中具体描述如下:
    WPWriteProtect(bit16ofCR0) — Whenset,inhibitssupervisor-levelprocedures from writing into read-only pages; when clear, allows supervisor-level procedures to write into read-only pages (regardless of the U/S bit setting;
                                                                                                                                                                                             see Section 4.1.3 and Section 4.6). This flag facilitates implementation of the copy-on-write method of creating a new process (forking) used by operating
    systems such as UNIX
    若将 CR0 的第 16 位若置位了则禁止超级权限,若清零了则允许超级权限。 这里的超级权限当然包括往内核空间写的权限。
    ￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼
    ￼4. 代码示例
    ￼#include <linux/module.h> #include <linux/kernel.h> #include <linux/init.h> #include <linux/unistd.h> #include <linux/time.h> #include <asm/uaccess.h> #include <linux/sched.h>
    //系统中未使用的系统调用号 unsigned int clear_and_return_cr0(void);
    #define __NR_syscall 223
#define SYS_CALL_TABLE_ADDRESS 0xc057e110 //步骤 2 中的 sys_call_table 的地址
    void setback_cr0(unsigned int val);
    int orig_cr0;
    unsigned long *sys_call_table = 0; static int (*anything_saved)(void);
    unsigned int clear_and_return_cr0(void) {
        unsigned int cr0 = 0; unsigned int ret;
        asm volatile ("movl %%cr0, %%eax" : "=a"(cr0)
                      ); ret = cr0;
        cr0 &= 0xfffeffff;
        asm volatile ("movl %%eax, %%cr0"
                      :
                      : "a"(cr0) );
        return ret; }
    ￼void setback_cr0(unsigned int val) {
        asm volatile ("movl %%eax, %%cr0" :
                      : "a"(val) );
    }
    asmlinkage long sys_mycall(void) {
        printk("This is my_syscall!\n"); return current->pid;
    }
    int init_addsyscall(void) {
        sys_call_table = (unsigned long *)SYS_CALL_TABLE_ADDRESS; anything_saved = (int(*)(void))(sys_call_table[__NR_syscall]); orig_cr0 = clear_and_return_cr0(); sys_call_table[__NR_syscall] = (unsigned long)&sys_mycall; setback_cr0(orig_cr0);
        return 0; }
    void exit_addsyscall(void) {
        orig_cr0 = clear_and_return_cr0(); sys_call_table[__NR_syscall] = (unsigned long)anything_saved; setback_cr0(orig_cr0);
        printk("call exit....\n");
    }
    module_init(init_addsyscall); module_exit(exit_addsyscall);
    MODULE_AUTHOR("WY"); MODULE_LICENSE("GPL");
    ￼Test.c
    ￼#include <stdio.h> #include <stdlib.h>
    int main() {
        Pid_t pid;
        Pid = syscall(223);
        printf("syscall result is %d\n", pid); return 0;
    }
    ￼Makefile
    ￼obj-m := syscall.o
    KERNELDIR := /lib/modules/$(shell uname –r)/build PWD := $(shell pwd)
modules:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
    ￼￼￼Note: 模块如果放到别的环境中是不能运行的,移植时,系统调用号和系统调用 表地址必须做出相应的修改
    ￼