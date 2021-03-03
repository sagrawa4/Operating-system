build: mysh.o
	gcc mysh.o -o mysh

mysh.o: mysh.c
	gcc mysh.c -c

run: build
	./mysh

clean:
	rm -rf mysh.o mysh