# Makefile for the ls project (ROLL_NO-OS-A02)
CC      = gcc
CFLAGS  = -Wall -Wextra -g
SRC     = src/ls-v1.0.0.c
OBJ     = obj/ls-v1.0.0.o
TARGET  = bin/ls

all: $(TARGET)

# link step
$(TARGET): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# compile step
obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

bin obj:
	mkdir -p $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
