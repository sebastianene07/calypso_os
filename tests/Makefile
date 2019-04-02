CFLAGS = -I../

all: compile run_test

compile:
	gcc $(CFLAGS) string_test.c ../utils/string.c -g -o string_test
	gcc string_test.c -o original_test

run_test:
	./string_test > my_output
	./original_test > original_output
	diff my_output original_output

clean:
	rm -f string_test original_test original_output my_output
