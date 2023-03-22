CC := gcc

CLIENT_FOLDER := ./client
CLIENT_TARGET := $(CLIENT_FOLDER)/chase-client
SERVER_FOLDER := ./server
SERVER_TARGET := $(SERVER_FOLDER)/chase-server

all: $(SERVER_TARGET) $(CLIENT_TARGET)

client: $(CLIENT_TARGET)
$(CLIENT_TARGET):
	make -C $(CLIENT_FOLDER)/

server: $(SERVER_TARGET)
$(SERVER_TARGET):  
	make -C $(SERVER_FOLDER)/

clean:
	make clean-server
	make clean-client

clean-client:
	make -C $(CLIENT_FOLDER) clean

clean-server:
	make -C $(SERVER_FOLDER) clean
	