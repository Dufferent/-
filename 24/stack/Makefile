PRO_NAME = demo
OBJ      = stack.o demo.o
CC       = gcc
#CC      = arm-linux-gnueabihf-gcc

$(PRO_NAME) : $(OBJ)
	$(CC) -o $@ $^

%.o  :   %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf ./*.o $(PRO_NAME) ./*.so

share:
	$(CC) -o stack.o -c -fPIC stack.c
	$(CC) -o demo.o -c -fPIC demo.c
	$(CC) -o libstack.so -shared stack.o
	$(CC) -o $(PRO_NAME) demo.o -lstack
  
