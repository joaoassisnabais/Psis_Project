SERVER_C_FILES	:= $(wildcard server/*.c) $(wildcard common/*.c)
BOTS_C_FILES	:= $(wildcard bots/*.c) $(wildcard common/*.c)
CLIENT_C_FILES	:= $(wildcard client/*.c) $(wildcard common/*.c)
SOURCE	= server.c prizes.c list.c chase.c message.c udp.c bots.c client.c
HEADER	= server.h prizes.h list.h chase.h defs.h message.h udp.h bots.h client.h
OUT	= ChaseThem
CC	 := gcc
FLAGS	 = -g3 -Wall
LFLAGS	 = 
# -g option enables debugging mode 
# -c flag generates object code for separate files


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

server: $(SERVER_C_FILES)
	$(CC) -g -o $(OUT)


# clean house
clean:
	rm -f $(OBJS) $(OUT)

# compile program with debugging information
debug: $(OUT)
	valgrind $(OUT)

# run program with valgrind for errors
valgrind: $(OUT)
	valgrind $(OUT)

# run program with valgrind for leak checks
valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full $(OUT)

# run program with valgrind for leak checks (extreme)
valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT)
