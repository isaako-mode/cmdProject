# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Command binaries
LS_BIN = ls
CAT_BIN = cat
ECHO_BIN = echo
MAIN_BIN = my_program

# Source files for each command
MAIN_SRCS = main.c commands.c
LS_SRCS = list.c commands.c
CAT_SRCS = cat.c commands.c
ECHO_SRCS = echo.c commands.c

# Object files (auto-generated from source files)
MAIN_OBJS = $(MAIN_SRCS:.c=.o)
LS_OBJS = $(LS_SRCS:.c=.o)
CAT_OBJS = $(CAT_SRCS:.c=.o)
ECHO_OBJS = $(ECHO_SRCS:.c=.o)


# Default rule: build all binaries
all: $(MAIN_BIN) $(LS_BIN) $(CAT_BIN) $(ECHO_BIN) 

# Rule to build the main binary (e.g., for the core application)
$(MAIN_BIN): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $(MAIN_BIN) $(MAIN_OBJS)

# Rule to build the ls binary
$(LS_BIN): $(LS_OBJS)
	$(CC) $(CFLAGS) -o $(LS_BIN) $(LS_OBJS)

#Echo rule
$(ECHO_BIN): $(ECHO_OBJS)
	$(CC) $(CFLAGS) -o $(ECHO_BIN) $(ECHO_OBJS)

#Cat rule
$(CAT_BIN): $(CAT_OBJS)
	$(CC) $(CFLAGS) -o $(CAT_BIN) $(CAT_OBJS)

# Generic rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(MAIN_OBJS) $(LS_OBJS) $(MAIN_BIN) $(LS_BIN) $(CAT_BIN) $(ECHO_BIN)

.PHONY: clean all
