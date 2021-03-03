#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>

#define TOTAL_PROCESS_BUFFER 1000

typedef struct 
{
	long pid;
	long ppid;
	long cpu;
	long state;
}ProcessInfo;

// These are define here 
// https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h#L75
char* task_state_to_string(long state){
	switch(state) {
		case 0:
			return "TASK_RUNNING";
		case 1:
			return "TASK_INTERRUPTIBLE";
		case 2:
			return "TASK_UNINTERRUPTIBLE";
		case 3:
			return "TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE";
		case 4:
			return "__TASK_STOPPED";
		case 8:
			return "__TASK_TRACED";
		case 15:
			return "TASK_NORMAL , __TASK_STOPPED , __TASK_TRACED";
		case 16:
			return "EXIT_DEAD";
		case 32:
			return "EXIT_ZOMBIE";
		case 63:
			return "TASK_RUNNING , TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE , __TASK_STOPPED , __TASK_TRACED , EXIT_ZOMBIE , EXIT_DEAD";
		case 64:
			return "TASK_DEAD";
		case 96:
			return "EXIT_ZOMBIE , EXIT_DEAD";
		case 128:
			return "TASK_WAKEKILL";	
		case 130:
			return "TASK_WAKEKILL , TASK_UNINTERRUPTIBLE";
		case 132:
			return "TASK_WAKEKILL , __TASK_STOPPED";
		case 136:
			return "TASK_WAKEKILL , __TASK_TRACED";
		case 256:
			return "TASK_WAKING";
		case 512:
			return "TASK_PARKED";
		case 1024:
			return "TASK_NOLOAD";
		case 1026:
			return "TASK_UNINTERRUPTIBLE , TASK_NOLOAD";
		case 2048:
			return "TASK_NEW";
		case 4096:
			return "TASK_STATE_MAX";
		default:
			return "INVALID";
	}
}

int main() 
{
    // Initilize values
    int fd, bytes_read = 0, i = 0, total_process = 0;
    ProcessInfo process[TOTAL_PROCESS_BUFFER];

    // Open the Process_List character device in READ ONLY mode.
    fd = open("/dev/process_list", O_RDONLY);
        
    if(fd < 0) 
    {
        perror("Error while opening process_list device driver.");
        return 0;
    }
    
    bytes_read = read(fd, process, TOTAL_PROCESS_BUFFER * sizeof(ProcessInfo));
	total_process = bytes_read / sizeof(ProcessInfo);

	printf("Total Bytes Read : %d\n", bytes_read);
	printf("Total Process : %d\n", total_process);

    if(bytes_read < 0)
    {
        perror("Error while reading from process_list device driver");
    }

    	// Scan and print all process for debugging.
	for(i = 0 ; i < total_process; ++i)
	{
		printf("Process_List: PID %lu PPID: %lu CPU: %lu STATE: %s \n", process[i].pid,
			process[i].ppid, process[i].cpu, task_state_to_string(process[i].state));
	}

    // Closing device driver.
    close(fd);

    return 0;
}