# Makefile for Your Project
# Compiler
CC = gcc
DEBUGGER = gf2

# Compiler flags
ifeq ($(OS),Windows_NT)
	CFLAGS = -ggdb -Wall -Wextra  -pedantic -O3
	EXECUTABLE = image-editor.exe
	LIBS = -lm -lWs2_32
else
	CFLAGS = -ggdb -Wall -Wextra -O3 -pedantic -fsanitize=address
	EXECUTABLE = image-editor
	LIBS = -lm
endif


# Libraries


#filters
FILTERS=test

# Source files
ifeq ($(OS),Windows_NT)
	SRC_FILES:= $(filter-out test.c test.o,$(wildcard src/*.c src/*/*.c))
else
	SRC_FILES:=$(shell find src -type f -iname *.c | grep -v $(FILTERS))
endif
# SRC_FILES = $(wildcard src/[^(test)]*.c src/*/[^(test)]*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)

# Output executable name


all: $(EXECUTABLE)

# Rule to compile C source files into object files
%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

$(EXECUTABLE): $(OBJ_FILES)
		$(CC) $(CFLAGS)  $^ -o $@ $(LIBS)


debug: compile
	$(DEBUGGER) ./image-editor

clean:
ifeq ($(OS),Windows_NT)
	rm ./src/*.o
	rm  ./src/*/*.o
	rm ./*.exe
else
	rm $(OBJ_FILES) $(EXECUTABLE)
endif

run: $(EXECUTABLE)
	./image-editor

test:
	echo -e "Done✅"

.PHONY: all compile debug clean run
