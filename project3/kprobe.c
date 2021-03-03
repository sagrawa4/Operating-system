#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include<linux/init.h>
#include<linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/uaccess.h>// copy to user
#include <linux/mm.h>
#include <linux/mm_types.h>
#include<linux/ptrace.h>
#include<linux/time.h>

#define MAX_SYMBOL_LEN	64
static char symbol[MAX_SYMBOL_LEN] = "handle_mm_fault";

static pid_t input_pid;
module_param(input_pid, int, 0);

//Allocate a kprobe structure for each probe needed
static struct kprobe kp = {	
	.symbol_name	= symbol,
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct timespec ctime;
	
	if(current->pid == input_pid)
	{
		ctime = current_kernel_time();
		pr_info("[kprobe] PID: %d Virtual Address: %lx TIME %ld \n", current->pid, regs->si,ctime.tv_nsec);
		pr_info("Parse,%d,%lx,%ld\n", current->pid, regs->si,ctime.tv_nsec);
	}
	return 0;
}

static int __init kprobe_init(void)
{
	int ret;
	printk(KERN_INFO "[kprobe] kprobe_init.. \n");

	kp.pre_handler = handler_pre;
	ret = register_kprobe(&kp);//register_kprobe() returns 0 on success or a negative errno otherwise
	if (ret < 0) 
    {
		pr_err(KERN_ERR "[kprobe] Register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	printk("[kprobe] Planted kprobe at PID: %d\n", input_pid);
	return 0;
}

static void __exit kprobe_exit(void)
{
	printk(KERN_INFO "[kprobe] kprobe_exit.. \n");
	unregister_kprobe(&kp);//unregister_kprobe removes the specified probe
	printk(KERN_INFO "[kprobe] kprob unregistered.. \n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);

MODULE_LICENSE("GPL");
