CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell
OBJS = shell.o shellFunctionsLecture.o shellFunctionsCommand.o
HEADERS = shell.h shellFunctionsLecture.h shellFunctionsCommand.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean