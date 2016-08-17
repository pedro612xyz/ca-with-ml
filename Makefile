
APP_NAME="didj"

CFLAGS=-DAPP_NAME="\"$(APP_NAME)\""

OBJS=main.o

all: $(OBJS)
	gcc -o $(APP_NAME) $(OBJS)


clean:
	rm -f $(APP_NAME) *.o
