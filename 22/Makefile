PRO_NAME = ts
OBJ      = demo.o tty.o
LIB      = libtty.so
LIB_NAME = tty
CC       = gcc
#CC       = arm-linux-gnueabihf-gcc

$(PRO_NAME) : $(OBJ)
	$(CC) -shared -o $(LIB) $(LIB_NAME).o
	$(CC) -o $@  $^ -l$(LIB_NAME)

%.o : %.c
	$(CC) -o $@ -fPIC -c $<

clean:
	rm -rf ./*.o $(PRO_NAME) $(LIB)