file.o:
		gcc -o ./disk/file.o -c ./disk/file.c

test: file.o
		gcc -o ./test.o -c ./test.c
		gcc -o ./test ./test.o ./disk/file.o
		./test

clean:
		rm -fr *.o
		rm -f ./test
