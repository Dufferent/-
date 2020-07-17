PRO_NAME = demo
OBJ      = queue.o demo.o
CC       = gcc
#CC      = arm-linux-gnueabihf-gcc

$(PRO_NAME) : $(OBJ)
	$(CC) -o $@ $^

%.o  :   %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf ./*.o $(PRO_NAME) ./*.so

share:
	$(CC) -o queue.o -c -fPIC queue.c
	$(CC) -o demo.o -c -fPIC demo.c
	$(CC) -o libqueue.so -shared queue.o
	$(CC) -o $(PRO_NAME) demo.o -lqueue
  
