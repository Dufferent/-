PRO_NAME = ffmpeg
OBJ      = demo.o
CC       = g++


$(PRO_NAME) : $(OBJ)
	$(CC) -o $@ $^ -lavcodec -lavdevice -lavformat -lavfilter -lswscale -lavutil -lswresample -lpostproc -Wl,-rpath=/usr/local/lib

%.o : %.cpp
	$(CC) -o $@ -c $< -lavcodec -lavdevice -lavformat -lavfilter -lswscale -lavutil -lswresample -lpostproc -Wl,-rpath=/usr/local/lib

clean:
	rm -rf ./*.o $(PRO_NAME)