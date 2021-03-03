## How to Compile and Run 

make run 

## To clean the binary

make clean


## What does not works 
 
-  Combination of filter chain and pipe command does not works 


## What works with sample test cases

- Single command 
    ls 

- Single command with arguments 
    ls -la

- Output redirection 
    ls -la > output.txt

- Input redirection 
    sort < output.txt

- Filter with two commands
    ls -la | grep mysh

- Filter with more than two commands 
    ls -la | grep mysh | sort

- Exit cmds
    exit
