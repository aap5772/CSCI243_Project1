CFLAGS =	-std=c99 -ggdb -Wall -Wextra -pedantic
CLIBFLAGS =	-lm 

SRCS = brace-topia.c display.c use_getopt.c
HDRS = brace-topia.h display.h

TARGET = brace-topia

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(CLIBFLAGS)

%.o: %.c $(HDRS)
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(OBJS) $(TARGET)

.PHONY: all clean
