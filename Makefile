exec:	main.c
	gcc main.c -o exec -Wall -lm -std=c99
run:	exec
	./exec iris.data
clean:
	rm exec exec.exe