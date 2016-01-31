all: copyit

copyit: copyit.c
	gcc -o copyit copyit.c

clean:
	rm copyit
