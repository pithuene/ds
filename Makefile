.PHONY: all
all: libds.a

ds/vec.o: ds/vec.c
	c89 -c ds/vec.c

mem/linalloc.o: mem/linalloc.c
	c89 -c mem/linalloc.c

mem/arenaalloc.o: mem/arenaalloc.c
	cc -c mem/arenaalloc.c

libds.a: ds/vec.o mem/linalloc.o mem/arenaalloc.o
	ar -rc libds.a ds/vec.o mem/linalloc.o mem/arenaalloc.o

types_test: types_test.c types.h
	c89 -o types_test types_test.c

ds/vec_test: ds/vec_test.c ds/vec.h libds.a
	c89 -o ds/vec_test ds/vec_test.c -L. -lds

mem/linalloc_test: mem/linalloc_test.c libds.a
	c89 -o mem/linalloc_test mem/linalloc_test.c -L. -lds

mem/arenaalloc_test: mem/arenaalloc_test.c libds.a
	c89 -o mem/arenaalloc_test mem/arenaalloc_test.c -L. -lds

.PHONY: test_types
test_types: types_test
	valgrind --leak-check=full ./types_test

.PHONY: ds/test_vec
ds/test_vec: ds/vec_test
	valgrind --leak-check=full ./ds/vec_test

.PHONY: mem/test_linalloc
mem/test_linalloc: mem/linalloc_test
	valgrind --leak-check=full ./mem/linalloc_test

.PHONY: mem/test_arenaalloc
mem/test_arenaalloc: mem/arenaalloc_test
	valgrind --leak-check=full ./mem/arenaalloc_test

.PHONY: run
run: ds/test_vec mem/test_linalloc test_types mem/test_arenaalloc

.PHONY: clean
clean:
	rm -f ./ds/vec.o; \
	rm -f ./ds/vec_test; \
	rm -f ./mem/linalloc.o; \
	rm -f ./mem/linalloc_test; \
	rm -f ./mem/arenaalloc.o; \
	rm -f ./mem/arenaalloc_test; \
	rm -f ./libds.a;
