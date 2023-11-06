#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "vect.h"
#include "tokens.h"

#define MAX_LINE_SIZE 256

char *last_command = NULL; // global variable for last command

void execute_shell_cmds(vect_t *tokenized_input);

void exec_cd(vect_t *input)
{
  if (vect_size(input) > 1 && vect_size(input) <= 2)
  {
    if (chdir(vect_get(input, 1)) != 0)
    {
      printf("cd: %s: No such file or directory\n", vect_get(input, 1));
    }
  }
  else
  {
    printf("cd: incorrect arg amount\n");
  }
}

void exec_prev()
{
  if (last_command)
  {
    printf("%s", last_command); // Print the previous command
    vect_t *prev_tokenized_input = tokenize(last_command);
    execute_shell_cmds(prev_tokenized_input);
    vect_delete(prev_tokenized_input);
  }
  else
  {
    printf("No previous command.\n");
  }
}

void exec_help()
{
  printf("Welcome to the mini-shell help utility. Below are the built-in commands and their usage:\n\n");
  printf("1. cd [directory]\n   Change the current directory to [directory]. If no directory is given, it prints an error message.\n\n");
  printf("2. prev\n   Executes the previous command entered into the shell.\n\n");
  printf("3. help\n   Displays this help information.\n\n");
  printf("4. source [file]\n   Reads and executes commands from [file] in the current shell environment.\n\n");
  printf("5. exit\n   Exits the shell with a Bye bye message.\n\n");
  printf("6. [command] > [file]\n   Redirects the output of [command] to [file], overwriting the existing contents.\n\n");
  printf("7. [command] < [file]\n   Takes the input for [command] from [file].\n\n");
  printf("8. [command1] | [command2]\n   Pipes the output of [command1] as the input to [command2].\n\n");
  printf("9. [command1] ; [command2]\n   Executes [command1] and then [command2], sequentially.\n\n");
  printf("Note: Commands not listed here are executed as external commands by the shell and the actual execution and behavior depend on the system's command binaries.\n");
}

void exec_source(char **args)
{
  char source_line[MAX_LINE_SIZE];
  FILE *file = file = fopen(args[1], "r");
  while (fgets(source_line, MAX_LINE_SIZE, file))
  {
    vect_t *tokenized_input = tokenize(source_line);
    execute_shell_cmds(tokenized_input);
    vect_delete(tokenized_input);
  }
  fclose(file);
}

void exec_redirect_out(vect_t *input)
{
  int loc_redirect = vect_find(input, ">");

  vect_t *left_cmd = partitioned_vector(input, 0, loc_redirect);
  vect_t *right_cmd = partitioned_vector(input, loc_redirect + 1, vect_size(input));

  int child_pid = fork();

  if (child_pid == 0)
  {
    // In child: replace the appropriate file descriptor to accomplish the redirect.
    int fd = open(vect_get(right_cmd, 0), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    close(1);
    dup(fd);
    execute_shell_cmds(left_cmd);
    exit(0);
  }
  else
  {
    waitpid(child_pid, NULL, 0);
    vect_delete(left_cmd);
    vect_delete(right_cmd);
  }
}

void exec_redirect_in(vect_t *input)
{
  int loc_redirect = vect_find(input, "<");

  vect_t *left_cmd = partitioned_vector(input, 0, loc_redirect);
  vect_t *right_cmd = partitioned_vector(input, loc_redirect + 1, vect_size(input));

  int child_pid = fork();

  if (child_pid == 0)
  {
    // In child: replace the appropriate file descriptor to accomplish the redirect.
    int fd = open(vect_get(right_cmd, 0), O_RDONLY);
    close(0);
    dup(fd);
    execute_shell_cmds(left_cmd);
    exit(0);
  }
  else
  {
    waitpid(child_pid, NULL, 0);
    vect_delete(left_cmd);
    vect_delete(right_cmd);
  }
}

void exec_pipe(vect_t *input)
{
  int loc_pipe = vect_find(input, "|");
  vect_t *left_cmd = partitioned_vector(input, 0, loc_pipe);
  vect_t *right_cmd = partitioned_vector(input, loc_pipe + 1, vect_size(input));

  int child_pid1 = fork();

  if (child_pid1 == 0)
  {
    // In child A: create a pipe
    int pipes_fd[2];
    assert(pipe(pipes_fd) == 0);
    int read_fd = pipes_fd[0];
    int write_fd = pipes_fd[1];

    // In Child A: Create a new child (CHild B)
    int child_pid2 = fork();

    if (child_pid2 == 0)
    {
      // In child B hook pipe to stdout, close other side
      close(read_fd);
      close(1);
      dup(write_fd);

      execute_shell_cmds(left_cmd);
      exit(0);
    }
    else
    {
      close(write_fd);
      close(0);
      dup(read_fd);

      waitpid(child_pid2, NULL, 0);

      execute_shell_cmds(right_cmd);
      exit(0);
    }
  }
  else
  {
    waitpid(child_pid1, NULL, 0);
    vect_delete(left_cmd);
    vect_delete(right_cmd);
  }
}

// executes the ; sequence function
void exec_sequence(vect_t *input)
{
  int loc_semi = vect_find(input, ";");
  vect_t *vect_left = partitioned_vector(input, 0, loc_semi);
  execute_shell_cmds(vect_left);
  vect_t *vect_right = partitioned_vector(input, loc_semi + 1, vect_size(input));
  execute_shell_cmds(vect_right);

  vect_delete(vect_left);
  vect_delete(vect_right);
}

void exec_external(char **args)
{
  pid_t pid = fork();

  if (pid == 0)
  {
    // Child process
    if (execvp(args[0], args) == -1)
    {
      printf("%s: command not found\n", args[0]);
      exit(0);
    }
  }
  else
  {
    // Parent process
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish
  }
}

char **init_args(vect_t *input)
{
  char **args = malloc((vect_size(input) + 1) * sizeof(char *));

  for (int i = 0; i < vect_size(input); ++i)
  {
    args[i] = input->data[i];
  }
  args[vect_size(input)] = NULL;

  return args;
}

void exec_built_in(vect_t *input)
{
  char **args = init_args(input);
  if (!args || !args[0])
  { // Check if args or args[0] is NULL
    if (args)
    {
      free(args);
    }
    return;
  }
  char *first = args[0];

  if (strcmp(first, "exit") == 0)
  {
    if (last_command)
    {
      free(last_command);
      last_command = NULL;
    }
    free(args);
    printf("Bye bye.\n");
    exit(0);
  }
  else if (strcmp(first, "source") == 0)
  {
    exec_source(args);
    free(args);
  }
  else if (strcmp(first, "prev") == 0)
  {

    free(args);
    exec_prev();
  }
  else if (strcmp(first, "help") == 0)
  {
    free(args);
    exec_help();
  }
  else if (strcmp(first, "cd") == 0)
  {
    free(args);
    exec_cd(input);
  }
  else
  {
    exec_external(args);
    free(args);
  }
}

void execute_shell_cmds(vect_t *tokenized_input)
{
  if (within_vect(tokenized_input, ";"))
  {
    exec_sequence(tokenized_input);
  }
  else if (within_vect(tokenized_input, "|"))
  {
    exec_pipe(tokenized_input);
  }
  else if (within_vect(tokenized_input, "<"))
  {
    exec_redirect_in(tokenized_input);
  }
  else if (within_vect(tokenized_input, ">"))
  {
    exec_redirect_out(tokenized_input);
  }
  else
  {
    exec_built_in(tokenized_input);
  }
}

// updates the prev command if the current command isn't prev
// needed to prevent infinite recursion
void update_prev(char *input_line)
{
  vect_t *prev_tokenizer = tokenize(input_line);

  if (strncmp(vect_get(prev_tokenizer, 0), "prev", 4) != 0)
  {
    // Free the last command before updating it to prevent memory leaks
    if (last_command != NULL)
    {
      free(last_command);
    }
    last_command = strdup(input_line);
  }
  vect_delete(prev_tokenizer);
}

int main(int argc, char *argv[])
{
  char input_line[MAX_LINE_SIZE];

  printf("Welcome to mini-shell.\n");
  if (argc == 1)
  {
    while (1)
    {
      printf("shell $ ");
      fflush(stdout);

      char *shell_input = fgets(input_line, MAX_LINE_SIZE, stdin);
      if (!shell_input)
      {
        printf("\n");
        break;
      }

      update_prev((char *)input_line);

      vect_t *tokenized_input = tokenize(input_line);

      execute_shell_cmds(tokenized_input);
      vect_delete(tokenized_input);
    }
  }

  return 0;
}

