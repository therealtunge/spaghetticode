CC := gcc
CFLAGS := -Wall -O0 -Iinclude -Werror -g
SRCS := $(wildcard *.c) $(wildcard */*.c)
OBJS := $(SRCS:.c=.o)
EXEC := vm

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

