.PHONY: all
all: libds.a

vec.o: vec.c
	c89 -c vec.c

linalloc.o: linalloc.c libds.a
	c89 -c linalloc.c -L. -lds

libds.a: vec.o linalloc.o libds.a
	ar -rc libds.a vec.o linalloc.o 

types_test: types_test.c types.h
	c89 -o types_test types_test.c

vec_test: vec_test.c vec.h libds.a
	c89 -o vec_test vec_test.c -L. -lds

linalloc_test: linalloc_test.c libds.a
	c89 -o linalloc_test linalloc_test.c -L. -lds

.PHONY: test_types
test_types: types_test
	valgrind --leak-check=full ./types_test

.PHONY: test_vec
test_vec: vec_test
	valgrind --leak-check=full ./vec_test

.PHONY: test_linalloc
test_linalloc: linalloc_test
	valgrind --leak-check=full ./linalloc_test

.PHONY: run
run: test_vec test_linalloc test_types

.PHONY: clean
clean:
	rm -f ./vec.o; \
	rm -f ./vec_test; \
	rm -f ./linalloc.o; \
	rm -f ./linalloc_test; \
	rm -f ./libds.a;
