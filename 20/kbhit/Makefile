share:
	gcc -fPIC -c ./kbhit.c -o kbhit.o -lpthread
	gcc -shared -o libkbhit.so ./kbhit.o -lpthread

static:
	gcc -fPIC -c ./kbhit.c -o kbhit.o -lpthread
	ar -rcs libkbhit.a ./kbhit.o

ts-static:
	gcc -o ts ./demo.c -lkbhit -lpthread

ts-shared:
	gcc -o ts ./demo.c -lkbhit

clean:
	rm -rf ./*.o ./*.a ./*.so ts