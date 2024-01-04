# Basic Shell
A basic shell written in C. test~

## Commands
- Sequencing Using ;
The behavior of ; is to execute the command on the left-hand side of the operator, and once it completes, execute the command on the right-hand side.

- Input Redirection <
A command may be followed by < and a file name. The command shall be run with the contents of the file replacing the standard input.

- Output Redirection >
A command may be followed by > and a file name. The command shall be run as normal, but the standard output should be captured in the given file. If the file exists, its original contents should be deleted (“truncated”) before output is written to it. If it does not exist, it should be created automatically. You do not need to implement output redirection for built-ins.

- Pipe |
The pipe operator | runs the command on the left hand side and the command on the right-hand side simultaneously and the standard output of the LHS command is redirected to the standard input of the RHS command. You do not have to support piping the output of built-ins.

- echo
- cd
- prev
- help
- exit
- source

## Visuals
![image](https://github.com/bderbs30/Basic-Shell/assets/124754518/44ea02cb-6a76-4032-bc02-7e1d9758248c)

