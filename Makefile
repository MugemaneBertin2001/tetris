CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -Iinclude -O2
LDFLAGS := -lncurses

SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)
TARGET  := tetris

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)