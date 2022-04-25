CFLAGS_DEBUG = -g -fprofile-arcs -ftest-coverage
CFLAGS = -std=c99 -pedantic -Wall -Wno-override-init-side-effects -Wno-unused-function -Werror $(CFLAGS_DEBUG)

TEST_SRCS = test/test_arr.c

vec/vec.o: vec/vec.c
	$(CC) $(CFLAGS) -c -o vec/vec.o vec/vec.c

libds.a: vec/vec.o
	ar -rc libds.a vec/vec.o

# Remove all previous coverage data
.PHONY: clean-coverage
clean-coverage:
	find . -name '*.gcda' -delete;
	rm -f ./coverage/*

test/test: libds.a clean-coverage
	$(CC) --coverage -o test/test test/test.c $(TEST_SRCS) libds.a test/munit/munit.c

.PHONY: test
test: test/test
	./test/test

.PHONY: coverage
coverage: test
	gcovr -s --html-details ./coverage/coverage.html --exclude test/test.c --exclude test/munit/munit.c --exclude test/test_arr.c test/test_vec.c
	xdg-open ./coverage/coverage.html

.PHONY: clean
clean:
	find . -name '*.gcda' -delete
	find . -name '*.gcno' -delete
	rm -f ./test/test
	rm -f ./vec/vec.o
	rm -f ./libds.a
