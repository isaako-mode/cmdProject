#ifndef FILERSYSTER_H
#define FILERSYSTER_H
#define FILE_SYSTER_PATH "/home/isaakpi/Desktop/c_projects/filesystem_project/source/my_program"


#include <unistd.h>

// Function prototypes
void setup_pipes();
void send_to_filesystem(const char* input);
void read_from_filesystem(char* output, size_t size);
void cleanup_pipes();

#endif // PIPES_H