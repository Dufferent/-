PRO_NAME = V4L2_APP
OBJ      = v4l2.o

CC       = gcc
#CC      = arm-linux-gnueabihf-gcc


$(PRO_NAME) : $(OBJ)
	$(CC) -o $@ $^ -std=gnu99

%.o : %.c
	$(CC) -o $@ -c $< -std=gnu99

clean:
	rm -rf ./*.o $(PRO_NAME) ./*.yuv