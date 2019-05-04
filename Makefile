CC=cc -ansi -pedantic -g

trabalho_ed: trabalho_ed.c
	${CC} trabalho_ed.c -o trabalho_ed

clean:
	rm -f *.o tmp.c tmp trabalho_ed
