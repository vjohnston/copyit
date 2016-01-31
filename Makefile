all: copyit copyrec

copyit: copyit.c
	gcc -o copyit copyit.c
copyrec: copyit_extracredit.c
	gcc -o copyrec copyit_extracredit.c
clean:
	rm copyit
	rm copyrec
