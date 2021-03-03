# Setup 
    - GCC for compiling the code. 
    - Sudo access to install kernel module and run code.

# Run instruction 
    - If you are running for first time `make run`
    - If you are running when module is already installed `make uninstall && make run`

# Expected Output for user process
    - After running user process you will see output in following format 
    
        ```
        Process_List: PID 18584 PPID: 2 CPU: 1 STATE: TASK_UNINTERRUPTIBLE , TASK_NOLOAD 
        Process_List: PID 18790 PPID: 2 CPU: 0 STATE: TASK_UNINTERRUPTIBLE , TASK_NOLOAD 
        Process_List: PID 23181 PPID: 13334 CPU: 0 STATE: TASK_INTERRUPTIBLE 
        Process_List: PID 24051 PPID: 23181 CPU: 0 STATE: TASK_INTERRUPTIBLE 
        Process_List: PID 24052 PPID: 24051 CPU: 1 STATE: TASK_RUNNING 
        ```

# See Kernal Logs 
    - If you want to see kernal logs specific to device driver run `make show_log`
