#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h> //close
#include <fcntl.h> //open
#include <sys/wait.h>

//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;


// builtin commands
#define EXIT_STR "exit"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99
#define INPUT_REINDIRECT_SYMBOL "<"
#define OUTPUT_REINDIRECT_SYMBOL ">"
#define PIPE_SYMBOL "|"
#define STDIN_FD 0
#define STDOUT_FD 1


FILE *fp; // file struct for stdin
char **tokens;
int token_count;
char *line;

void initialize()
{

	// allocate space for the whole line
	assert( (line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

	// allocate space for individual tokens
	assert( (tokens = malloc(sizeof(char*)*MAX_TOKENS)) != NULL);

	// open stdin as a file pointer 
	assert( (fp = fdopen(STDIN_FILENO, "r")) != NULL);

}

void print_tokens()
{
	int i;
	for(i = 0; tokens[i] != NULL; ++i)
	{
		printf("Token[%d] : %s\n", i, tokens[i]);
	}
}

void print_new_tokens(char **new_tokens)
{
	int i;
	for(i = 0; new_tokens[i] != NULL; ++i)
	{
		printf("New Token[%d] : %s\n", i, new_tokens[i]);
	}
}

void tokenize(char * string)
{
	token_count = 0;
	int size = MAX_TOKENS;
	char *this_token;

    // Clean up old tokens
    int counter = 0;
    while(tokens[counter] != NULL)
    {
        tokens[counter] = NULL;
        ++counter;
    }

    while ( (this_token = strsep( &string, " \t\v\f\n\r")) != NULL) {
		if (*this_token == '\0') continue;
		tokens[token_count] = this_token;
		token_count++;
		// if there are more tokens than space ,reallocate more space
		if(token_count >= size){
			size*=2;
			assert ( (tokens = realloc(tokens, sizeof(char*) * size)) != NULL);
		}
	}
}

void read_command() 
{
	// getline will reallocate if input exceeds max length
	assert( getline(&line, &MAX_LINE_LEN, fp) > -1); 
    tokenize(line);
}

int is_input_reindirect_cmd()
{
	int i;
	for(i = 0; i < token_count; ++i)
	{
		if(strcmp(tokens[i], INPUT_REINDIRECT_SYMBOL) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int is_output_reindirect_cmd()
{
	int i;
	for(i = 0; i < token_count; ++i)
	{
		if(strcmp(tokens[i], OUTPUT_REINDIRECT_SYMBOL) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int is_pipe_cmd()
{
	int i;
	for(i = 0; i < token_count; ++i)
	{
		if(strcmp(tokens[i], PIPE_SYMBOL) == 0)
		{
			return 1;
		}
	}
	return 0;	
}

void run_input_reindirect_cmd()
{
	pid_t pid;

	// Keep track of < symbol to get name of the file
	int i, indirect_symb_index;
	
	for(i = 0; i < token_count; ++i)
	{
		if(strcmp(tokens[i], INPUT_REINDIRECT_SYMBOL) == 0)
		{
			indirect_symb_index = i;	
		}
	}

	char *filename = tokens[indirect_symb_index + 1];
	pid = fork();

	if(pid < 0)
	{
		perror("[run_input_reindirect_cmd] Fork Failed!");
	}
	
	// PID == 0 if fork is successful, below code is executed in child process.
	if(pid == 0 )
	{
		//printf("Opening file : %s\n", filename);
		int file = open(filename, O_RDONLY);

		if(file == -1)
		{
			fprintf(stderr, "Error while opening file %s\n", filename);
			return;
		}
		else 
		{
			int dup2_out = dup2(file, STDIN_FD);
			close(file);

			if(dup2_out == STDIN_FD)
			{
				// Remove {<, filename} from token list as we are using 
				// since execp command we need to only pass command and arg list
				tokens[indirect_symb_index] = NULL;
				if(execvp(tokens[0], tokens) == -1)
				{
					fprintf(stderr, "execvp failed executing input indirection cmd!\n");
				} 	
			}
			else
			{
				perror("Error in Dup\n");
            	exit(1);
			}
		}
	}

	// If pid > 0, below code is executed in parent process
	// wait for cmd to execute in child process.
	if (pid > 0)
	{
		int status;
        if ((waitpid(pid, &status, 0)) == -1) 
		{
            perror("Wait Failed\n");
            exit(1);
        }
    }
}

void run_output_reindirect_cmd()
{
	pid_t pid;

	// Keep track of > symbol to get name of the file
	int i, indirect_symb_index;

	for(i = 0; i < token_count; ++i)
	{
		if(strcmp(tokens[i], OUTPUT_REINDIRECT_SYMBOL) == 0)
		{
			indirect_symb_index = i;
		}
	}

	char *filename = tokens[indirect_symb_index + 1];
	pid = fork();

	if(pid < 0)
	{
		perror("[run_output_reindirect_cmd] Fork Failed!");
	}
	
	// PID == 0 if fork is successful, below code is executed in child process.
	if(pid == 0 )
	{
		int file = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0755);

		if(file == -1)
		{
			fprintf(stderr, "Error while opening file %s\n", filename);
			return;
		}
		else
		{
			int dup2_out= dup2(file, STDOUT_FD);
			close(file);

			if(dup2_out == STDOUT_FD)
			{
				// Remove {>, filename} from token list as we are using 
				// since execp command we need to only pass command and arg list
				tokens[indirect_symb_index] = NULL;
				if(execvp(tokens[0], tokens) == -1)
				{
					fprintf(stderr, "execvp failed executing input indirection cmd!\n");
				}
			}
			else
			{
				perror("Error in Dup\n");
            	exit(1);
			}
		}
	}

	// If pid > 0, below code is executed in parent process
	// wait for cmd to execute in child process.
	if (pid > 0)
	{
		int status;
        if ((waitpid(pid, &status, 0)) == -1) 
		{
            perror("Wait Failed\n");
            exit(1);
        }
    }
}

void run_simple_cmd()
{
	pid_t pid;
	pid = fork();
	
	if(pid < 0)
    {     
		perror("[run_simple_cmd] Fork Failed!");
        exit(1);
    }
    
	// Child executes
	if (pid == 0)
    {
        // for the child process
        if (execvp(*tokens, tokens) == -1)
        {
            fprintf(stderr, "execvp failed for cmd %s!\n", tokens[0]);
            exit(1);
        }
    }

	// Parent executes
    if (pid > 0)
	{
		int status;
        if ((waitpid(pid, &status, 0)) == -1) 
		{
            perror("Wait Failed\n");
            exit(1);
        }
    }
}

int run_pipe_single_cmd(int input, int cmd_start_idx, int cmd_end_idx)
{
    // Create new command tokens based on start and end index
    char **new_tokens = malloc(sizeof(char*)*MAX_TOKENS);
    int i = 0, j=0;
    for(i = cmd_start_idx; i<= cmd_end_idx; ++i)
    {
        new_tokens[j] = tokens[i];
        ++j;
    }
    
    // Idx 0 is first command
    // Idx token_count -1 is last command
    int first = 0, last = 0;
    if (cmd_start_idx == 0)
    {
        first = 1;
    }

    if(cmd_end_idx == (token_count - 1))
    {
        last = 1;
    }

    int pipe_fd[2];
	if(pipe(pipe_fd) == -1)
	{
		perror("Error creating Pipe!\n");
		exit(1);
	}

	int pid = fork();
	if(pid < 0)
    {     
		perror("[run_pipe_cmd] Fork Failed!");
        exit(1);
    }

	if(pid == 0)
	{
		if(first == 1 && last == 0)
		{
            // First command only writes output to stdout.
			//printf("First Pipe Command!\n");
			dup2(pipe_fd[1], STDOUT_FD);
		}
		else if(first == 0 && last == 1)
		{
            // Last command only reads from stdin.
            //printf("Last Pipe Command!\n");
			dup2(input, STDIN_FD);
		}
		else
        {
            // Middle command reads from stdin and output to stdout.
            //printf("Middle Pipe Command!\n");
			dup2(input, STDIN_FD);
			dup2(pipe_fd[1], STDOUT_FD);
        }
    
        // Child Executes
		if (execvp(*new_tokens, new_tokens) == -1)
        {
            fprintf(stderr, "execvp failed for cmd %s!\n", new_tokens[0]);
            exit(1);
        }
	}

    if (pid > 0)
	{
		int status;
        if ((waitpid(pid, &status, WUNTRACED)) == -1) 
		{
            perror("Wait Failed\n");
            exit(1);
        }
        close(pipe_fd[1]);
    }
	// returns the read file descriptor to pass input to next command 
	return pipe_fd[0];
}

void run_pipe_cmd()
{
    int cmd_start_idx = 0, cmd_input = 0, i = 0;
    for(i = 0; i < token_count; ++i)
    {
		// Split command by |
        if(strcmp(tokens[i], PIPE_SYMBOL) == 0)
        {
			// Run command 
            cmd_input = run_pipe_single_cmd(cmd_input, cmd_start_idx, i - 1);
            cmd_start_idx = i +1;
        }
    }
    run_pipe_single_cmd(cmd_input, cmd_start_idx, token_count - 1);   
}


int run_command() 
{
	if (strcmp( tokens[0], EXIT_STR ) == 0)
	{
		return EXIT_CMD;
	}
	else if(is_pipe_cmd() == 1)
	{
		run_pipe_cmd();
	}
	else if(is_input_reindirect_cmd() == 1)
	{
		// if only input reindirect
		run_input_reindirect_cmd();
	}
	else if(is_output_reindirect_cmd() == 1)
	{
		// If only output reindirect
		run_output_reindirect_cmd();
	}
	else
	{
		run_simple_cmd();
	}

	return UNKNOWN_CMD;
}

int main()
{
	initialize();

	while(1)
    {
		printf("mysh> ");
		read_command();
        if( run_command() == EXIT_CMD)
        {
            break;
        }
	}
	return 0;
}