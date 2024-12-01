#ifndef PIPES_H
#define PIPES_H

#include <unistd.h>

// Function prototypes
void setup_pipes();
void send_to_filesystem(const char* input);
void read_from_filesystem(char* output, size_t size);
void cleanup_pipes();

#endif // PIPES_H