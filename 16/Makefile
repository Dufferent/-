PRO_NAME = LCD_APP
OBJ      = lcd.o

#CC       = gcc
CC        = arm-linux-gnueabihf-gcc


$(PRO_NAME) : $(OBJ)
	$(CC) -o $@ $^ -std=gnu99

%.o : %.c
	$(CC) -o $@ -c $< -std=gnu99

clean:
	rm -rf ./*.o $(PRO_NAME)