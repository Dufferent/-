PRO1_NAME = scpsvr
PRO2_NAME = scpc
OBJ1      = myscp.o
OBJ2      = scpc.o
CC       = g++
#CC       = arm-linux-gnueabihf-gcc

$(PRO1_NAME) : $(OBJ1)
	$(CC) -o $@ $^

%.o : %.cpp
	$(CC) -o $@ -c $<

client:
	$(CC) -o $(PRO2_NAME) ./scpc.cpp

clean:
	rm -rf ./*.o $(PRO1_NAME) $(PRO2_NAME)