# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Command binaries
LS_BIN = ls
CAT_BIN = cat
ECHO_BIN = echo
MV_BIN = mv
TOUCH_BIN = touch
MKDIR_BIN = mkdir
GREP_BIN = grep
MAIN_BIN = my_program
#QUEUE_BIN = queue

# Source files for each command
MAIN_SRCS = main.c commands.c
LS_SRCS = list.c commands.c
CAT_SRCS = cat.c commands.c
ECHO_SRCS = echo.c commands.c
MV_SRCS = mv.c commands.c
TOUCH_SRCS = touch.c commands.c
MKDIR_SRCS = mkdir.c commands.c
GREP_SRCS = grep.c commands.c queue.c
#QUEUE_SRCS = queue.c




# Object files (auto-generated from source files)
MAIN_OBJS = $(MAIN_SRCS:.c=.o)
LS_OBJS = $(LS_SRCS:.c=.o)
CAT_OBJS = $(CAT_SRCS:.c=.o)
ECHO_OBJS = $(ECHO_SRCS:.c=.o)
MV_OBJS = $(MV_SRCS:.c=.o)
TOUCH_OBJS = $(TOUCH_SRCS:.c=.o)
MKDIR_OBJS = $(MKDIR_SRCS:.c=.o)
GREP_OBJS = $(GREP_SRCS:.c=.o)
#QUEUE_OBJS = $(QUEUE_SRCS:.c=.o)



# Default rule: build all binaries
all: $(MAIN_BIN) $(LS_BIN) $(CAT_BIN) $(ECHO_BIN) $(MV_BIN) $(TOUCH_BIN) $(MKDIR_BIN) $(GREP_BIN) $(QUEUE_BIN)
	$(CC) $(CFLAGS) -o $(LS_BIN) $(LS_OBJS)

$(MAIN_BIN): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $(MAIN_BIN) $(MAIN_OBJS)

#Echo rule
$(ECHO_BIN): $(ECHO_OBJS)
	$(CC) $(CFLAGS) -o $(ECHO_BIN) $(ECHO_OBJS)

#Cat ruleS
$(CAT_BIN): $(CAT_OBJS)
	$(CC) $(CFLAGS) -o $(CAT_BIN) $(CAT_OBJS)

#mv rules
$(MV_BIN): $(MV_OBJS)
	$(CC) $(CFLAGS) -o $(MV_BIN) $(MV_OBJS)

#touch rules
$(TOUCH_BIN): $(TOUCH_OBJS)
	$(CC) $(CFLAGS) -o $(TOUCH_BIN) $(TOUCH_OBJS)

#mkdir rules
$(MKDIR_BIN): $(MKDIR_OBJS)
	$(CC) $(CFLAGS) -o $(MKDIR_BIN) $(MKDIR_OBJS)

#mkdir rules
$(GREP_BIN): $(GREP_OBJS)
	$(CC) $(CFLAGS) -o $(GREP_BIN) $(GREP_OBJS)

#mkdir rules
#$(QUEUE_BIN): $(QUEUE_OBJS)
#	$(CC) $(CFLAGS) -o $(QUEUE_BIN) $(QUEUE_OBJS)

$(LS_BIN): $(LS_OBJS)
	$(CC) $(CFLAGS) -o $(LS_BIN) $(LS_OBJS)


# Generic rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(MAIN_OBJS) $(LS_OBJS) $(MAIN_BIN) $(LS_BIN) $(CAT_BIN) $(ECHO_BIN) $(MV_BIN) $(TOUCH_BIN) $(MKDIR_BIN) $(GREP_BIN) $(QUEUE_BIN)