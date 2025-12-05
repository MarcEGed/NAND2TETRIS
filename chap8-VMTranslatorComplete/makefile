CC = gcc

CFLAGS = -I src -I -Wall -Wextra -O2

SRCS =  src/parser.c \
		src/CodeWriter.c \
		main.c
       
TARGET = VM2ASM.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean