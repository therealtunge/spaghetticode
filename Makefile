CC := gcc
CFLAGS := -Wall -O0 -Iinclude -Werror -g -std=c23
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

java:
	javac example.java
	~/Android/Sdk/build-tools/36.1.0/d8 example.class

.PHONY: all clean java

