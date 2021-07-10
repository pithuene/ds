.PHONY: all
all: libds.a

ds/vec.o: ds/vec.c
	c89 -c ds/vec.c

memory/linalloc.o: memory/linalloc.c
	c89 -c memory/linalloc.c

memory/arenaalloc.o: memory/arenaalloc.c
	cc -c memory/arenaalloc.c

libds.a: ds/vec.o memory/linalloc.o memory/arenaalloc.o
	ar -rc libds.a ds/vec.o memory/linalloc.o memory/arenaalloc.o

types_test: types_test.c types.h
	c89 -o types_test types_test.c

ds/vec_test: ds/vec_test.c ds/vec.h libds.a
	c89 -o ds/vec_test ds/vec_test.c -L. -lds

memory/linalloc_test: memory/linalloc_test.c libds.a
	c89 -o memory/linalloc_test memory/linalloc_test.c -L. -lds

memory/arenaalloc_test: memory/arenaalloc_test.c libds.a
	c89 -o memory/arenaalloc_test memory/arenaalloc_test.c -L. -lds

.PHONY: test_types
test_types: types_test
	valgrind --leak-check=full ./types_test

.PHONY: ds/test_vec
ds/test_vec: ds/vec_test
	valgrind --leak-check=full ./ds/vec_test

.PHONY: memory/test_linalloc
memory/test_linalloc: memory/linalloc_test
	valgrind --leak-check=full ./memory/linalloc_test

.PHONY: memory/test_arenaalloc
memory/test_arenaalloc: memory/arenaalloc_test
	valgrind --leak-check=full ./memory/arenaalloc_test

.PHONY: run
run: ds/test_vec memory/test_linalloc test_types memory/test_arenaalloc

.PHONY: clean
clean:
	rm -f ./ds/vec.o; \
	rm -f ./ds/vec_test; \
	rm -f ./memory/linalloc.o; \
	rm -f ./memory/linalloc_test; \
	rm -f ./memory/arenaalloc.o; \
	rm -f ./memory/arenaalloc_test; \
	rm -f ./libds.a;
