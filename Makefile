CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell
OBJS = shell.o shellFunctionsLecture.o shellFunctionsCommand.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean