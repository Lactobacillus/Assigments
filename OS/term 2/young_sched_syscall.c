/*
 *  young_sched_syscall.c
 *  Operating System 2016
 *  Author : Cho Youngwoo
 *  2016.05.07
 */

#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/types.h>

pid_t youngFastPID;
pid_t youngSlowPID;

EXPORT_SYMBOL(youngFastPID);
EXPORT_SYMBOL(youngSlowPID);

asmlinkage int sys_young_fastsched(pid_t pid){

		youngFastPID = pid;
		printk("========== fast scheduled : %d ==========\n", pid);

	return 0;
}

asmlinkage int sys_young_slowsched(pid_t pid){

		youngSlowPID = pid;
		printk("========== slow scheduled : %d ==========\n", pid);	

	return 0;
}
