.PHONY: all
all: libds.a

ds/vec.o: ds/vec.c
	c89 -c -o ds/vec.o ds/vec.c

mem/linalloc.o: mem/linalloc.c
	c89 -c -o mem/linalloc.o mem/linalloc.c

ds/map.o: ds/map.c
	c89 -c -o ds/map.o ds/map.c

mem/arenaalloc.o: mem/arenaalloc.c
	cc -c -o mem/arenaalloc.o mem/arenaalloc.c

libds.a: ds/vec.o ds/map.o mem/linalloc.o mem/arenaalloc.o
	ar -rc libds.a ds/vec.o ds/map.o mem/linalloc.o mem/arenaalloc.o

types_test: types_test.c types.h
	c89 -o types_test types_test.c

ds/vec_test: ds/vec_test.c ds/vec.h libds.a
	c89 -o ds/vec_test ds/vec_test.c -L. -lds

mem/linalloc_test: mem/linalloc_test.c libds.a
	c89 -o mem/linalloc_test mem/linalloc_test.c -L. -lds

ds/map_test: ds/map_test.c ds/map.h libds.a
	c89 -o ds/map_test ds/map_test.c -L. -lds

mem/arenaalloc_test: mem/arenaalloc_test.c libds.a
	c89 -o mem/arenaalloc_test mem/arenaalloc_test.c -L. -lds

.PHONY: test_types
test_types: types_test
	valgrind --leak-check=full ./types_test

.PHONY: test_vec
test_vec: ds/vec_test
	valgrind --leak-check=full ./ds/vec_test

.PHONY: test_linalloc
test_linalloc: mem/linalloc_test
	valgrind --leak-check=full ./mem/linalloc_test

.PHONY: test_map
test_map: ds/map_test
	valgrind --leak-check=full ./ds/map_test

.PHONY: test_arenaalloc
test_arenaalloc: mem/arenaalloc_test
	valgrind --leak-check=full ./mem/arenaalloc_test

.PHONY: run
run: test_vec test_map test_linalloc test_types test_arenaalloc

.PHONY: clean
clean:
	rm -f ./ds/vec.o; \
	rm -f ./ds/vec_test; \
	rm -f ./ds/map.o; \
	rm -f ./ds/map_test; \
	rm -f ./mem/linalloc.o; \
	rm -f ./mem/linalloc_test; \
	rm -f ./mem/arenaalloc.o; \
	rm -f ./mem/arenaalloc_test; \
	rm -f ./libds.a;
