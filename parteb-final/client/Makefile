CC := gcc
TARGET := chase-client
FLAGS := -Wall -pedantic -g
LIBS := -lncurses -lpthread

C_FILES := $(wildcard *.c) $(wildcard ../common/*.c)
H_FILES := $(wildcard *.h) $(wildcard ../common/*.h)

$(TARGET): $(C_FILES) $(H_FILES)
	$(CC) $(FLAGS) -o $@ $(C_FILES) $(LIBS)

clean:
	rm $(TARGET)