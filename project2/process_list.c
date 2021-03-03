#include<linux/init.h>
#include<linux/module.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
// Needed to run on virtual machine
#include<linux/uaccess.h>
#include<linux/sched/signal.h>
// Needed to run on local machine
//#include<asm/uaccess.h>
//#include<linux/sched.h>
#include<linux/slab.h>
#include<linux/kernel.h>

#define TOTAL_PROCESS_BUFFER 1000

typedef struct 
{
	long pid;
	long ppid;
	long cpu;
	long state;
}ProcessInfo;

ProcessInfo process[TOTAL_PROCESS_BUFFER];

int TOTAL_PROCESS = 0;

// Declare File operations functions
static ssize_t process_list_read(struct file *file, char __user * out, size_t size, loff_t * off);
static int process_list_open(struct inode*,struct file *);
static int process_list_close(struct inode*, struct file *);

// Declare file operations structs
static struct file_operations process_list_fops = {
	.owner = THIS_MODULE,	
	.open = process_list_open,
	.read = process_list_read,
	.release = process_list_close
};

// Declare a device struct
static struct miscdevice process_list_device  = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "process_list",
	.fops = &process_list_fops
};

// Register process list device driver with kernel during initialization the module
static int __init process_list_init(void){
    int reg_status;
	printk(KERN_INFO "Process_List: Registering the Process List Device\n");
	reg_status = misc_register(&process_list_device);
	if(reg_status < 0){
		printk(KERN_ALERT "Process_List: Registration FAILED!");
		return 0;
	}
	return 0;
}

// Deregister proces list device driver from kernel while removing the module
static void __exit process_list_exit(void) {
	printk(KERN_INFO "Process_List: Deregistering the Process List Device\n");
	misc_deregister(&process_list_device);
}

// Read operation for device driver.
static ssize_t process_list_read(struct file *file, char *out, size_t size, loff_t * off)
{
	int status;
	int bytes_read = 0;

	printk(KERN_INFO "Process_List: Reading the Prcoess Information...\n");

	if (!access_ok(VERIFY_WRITE, out, TOTAL_PROCESS))
	{
		printk(KERN_ALERT "Process_List_READ: Invalid Output parameter !");
	}

	if (size < TOTAL_PROCESS)
	{
		bytes_read = size * sizeof(ProcessInfo);
	}
	else
	{
		bytes_read = TOTAL_PROCESS * sizeof(ProcessInfo);
	}

	status = copy_to_user(out, process, bytes_read);
	
	if(status !=0)
	{	
		printk(KERN_ALERT "Process_List: Error in copying data to user!");
		return -EFAULT;
	}
	return bytes_read;
}

// List operation for device driver.
static int process_list_open(struct inode * inode,struct file * file) {
	struct task_struct *task;
	int current_process = 0;
	
	printk(KERN_INFO "Process_List: Device Opened\n");
	
	// Get total number of process
	for_each_process(task) {
    	++TOTAL_PROCESS;
	}

	if(TOTAL_PROCESS > TOTAL_PROCESS_BUFFER)
	{
		printk(KERN_ALERT "Process_List Only supports 1000 process");
	}
	
	// Populate array for each process with info
	for_each_process(task) 
	{
		process[current_process].pid = task->pid;
		process[current_process].ppid = task->parent->pid;
		process[current_process].cpu = task_cpu(task);
		process[current_process].state = task->state;
		++current_process;
	}

	// Scan and print all process for debugging.
	for(current_process = 0 ; current_process < TOTAL_PROCESS; ++current_process)
	{
		printk(KERN_INFO "Process_List: PID %lu PPID: %lu CPU: %lu STATE: %lu \n", process[current_process].pid,
			process[current_process].ppid, process[current_process].cpu, process[current_process].state);
	}
	
	// Print total process for debugging.
	printk(KERN_INFO "Process_List: Device Opened %d\n", TOTAL_PROCESS);	
	return 0;
}

// Close operation for device driver.
static int process_list_close(struct inode * inode,struct file *file) {
	printk(KERN_INFO "Process_List: Device Closed!\n");
	return 0;
}

// Associate modules.
module_init(process_list_init);
module_exit(process_list_exit);