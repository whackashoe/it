CFLAGS=-O1 -Wall -Werror -pedantic
SOURCES= $(wildcard ../src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
TARGET=$(lastword $(subst /, ,$(realpath .)))
CC=gcc

all: $(TARGET)
	@echo it built

$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) -o it $(OBJECTS)

clean:
	rm -rf $(OBJECTS) it
