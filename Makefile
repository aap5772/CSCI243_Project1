CC = gcc
CFLAGS = -std=c99 -ggdb -Wall -Wextra -pedantic
CLIBFLAGS = -lm

# List of source files
SRCS = brace-topia.c display.c
HDRS = brace-topia.h display.h

# Output executable
TARGET = brace-topia

# Object files
OBJS = $(SRCS:.c=.o)

all: $(TARGET) use_getopt

$(TARGET): $(OBJS)
        $(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(CLIBFLAGS)

use_getopt: use_getopt.c
        $(CC) $(CFLAGS) -o use_getopt use_getopt.c $(CLIBFLAGS)

# Compile C files into object files
%.o: %.c $(HDRS)
        $(CC) $(CFLAGS) -c $< -o $@

clean:
        rm -f $(OBJS) $(TARGET) use_getopt

.PHONY: all clean
