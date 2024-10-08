#ifdef COMMANDS_H
#define COMMANDS_H
#define MAX_OUTPUT_SIZE 200

void run_commands(char* cmd, char args[]);
char* cd(char args[]);
void list(char args[]);
void escape();

#endif