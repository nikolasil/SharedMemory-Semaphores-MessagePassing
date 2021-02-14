p1:
	gcc -o p1 p1.c shared_memory.c semaphore.c util.c -lpthread -lcrypto -w
p2:
	gcc -o p2 p2.c shared_memory.c semaphore.c util.c -lpthread -lcrypto -w
chan:
	gcc -o chan channel.c shared_memory.c semaphore.c util.c -lpthread -lcrypto -w
all:
	make p1 && make p2 && make chan
clean:
	rm p1 p2 chan