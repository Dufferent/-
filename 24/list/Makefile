PRO_NAME = demo
OBJ      = list.o demo.o
CC       = gcc
#CC      = arm-linux-gnueabihf-gcc

$(PRO_NAME) : $(OBJ)
	$(CC) -o $@ $^

%.o  :   %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf ./*.o $(PRO_NAME) ./*.so

share:
	$(CC) -o list.o -c -fPIC list.c
	$(CC) -o demo.o -c -fPIC demo.c
	$(CC) -o liblist.so -shared list.o
	$(CC) -o $(PRO_NAME) demo.o -llist
  
