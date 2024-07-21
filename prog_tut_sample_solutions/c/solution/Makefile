CC      = gcc
CFLAGS  = -Wall -Wpedantic -Werror -std=gnu99
LIBS    = -lpthread -lssl -lcrypto
INCLUDE = 

# macOS specific, where openssl@3 has been installed using homebrew
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	INCLUDE += -I/opt/homebrew/opt/openssl@3/include
	LIBS += -L/opt/homebrew/opt/openssl@3/lib
endif

.PHONY: all clean 

all: client server

clean:
	rm -f client server 
	rm -f *.o 
	rm -rf *.dSYM

client: client.c 
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCLUDE)

server: server.c bank.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(INCLUDE)

bank.o: bank.c bank.h
	$(CC) $(CFLAGS) -c bank.c
