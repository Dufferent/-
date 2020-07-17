PRO1_NAME = server
PRO2_NAME = client
OBJ1      = server.o
OBJ2      = client.o
CC       = gcc
#CC       = arm-linux-gnueabihf-gcc

$(PRO1_NAME) : $(OBJ1)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -o $@ -c $<

client:
$(PRO2_NAME) : $(OBJ2)
	$(CC) -o $@ $^ -L./queue -lqueue

%.o : %.c
	$(CC) -o $@ -c $< -L./queue -lqueue

clean:
	rm -rf ./*.o $(PRO1_NAME) $(PRO2_NAME)