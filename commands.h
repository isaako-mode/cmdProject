#ifdef COMMANDS_H
#define COMMANDS_H
#define MAX_OUTPUT_SIZE 200
#define MY_STDOUT STDOUT_FILENO


void run_commands(char* cmd, char args[]);
char* cd(char args[]);
void list(char args[]);
void clearScreen();
void cat(char** args);


#endif