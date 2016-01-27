all: copyit

copyit: copyit.c
	gcc -o copyit copyit.c -lssl

clean:
	rm copyit
