.PHONY: all
all: libds.a

ds/vec.o: ds/vec.c
	$(CC) -c -o ds/vec.o ds/vec.c

mem/linalloc.o: mem/linalloc.c
	$(CC) -c -o mem/linalloc.o mem/linalloc.c

ds/map.o: ds/map.c
	$(CC) -c -o ds/map.o ds/map.c

mem/arenaalloc.o: mem/arenaalloc.c
	$(CC) -c -o mem/arenaalloc.o mem/arenaalloc.c

mem/poolalloc.o: mem/poolalloc.c
	$(CC) -c -o mem/poolalloc.o mem/poolalloc.c

libds.a: ds/vec.o ds/map.o mem/linalloc.o mem/arenaalloc.o mem/poolalloc.o
	ar -rc libds.a ds/vec.o ds/map.o mem/linalloc.o mem/arenaalloc.o mem/poolalloc.o

types_test: types_test.c types.h
	$(CC) -o types_test types_test.c

ds/vec_test: ds/vec_test.c ds/vec.h libds.a
	$(CC) -o ds/vec_test ds/vec_test.c -L. -lds

mem/linalloc_test: mem/linalloc_test.c libds.a
	$(CC) -o mem/linalloc_test mem/linalloc_test.c -L. -lds

ds/map_test: ds/map_test.c ds/map.h libds.a
	$(CC) -o ds/map_test ds/map_test.c -L. -lds

mem/arenaalloc_test: mem/arenaalloc_test.c libds.a
	$(CC) -o mem/arenaalloc_test mem/arenaalloc_test.c -L. -lds

mem/poolalloc_test: mem/poolalloc_test.c libds.a
	$(CC) -o mem/poolalloc_test mem/poolalloc_test.c -L. -lds

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

.PHONY: test_poolalloc
test_poolalloc: mem/poolalloc_test
	valgrind --leak-check=full ./mem/poolalloc_test

.PHONY: run
run: test_vec test_map test_linalloc test_types test_arenaalloc test_poolalloc

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
	rm -f ./mem/poolalloc.o; \
	rm -f ./mem/poolalloc_test; \
	rm -f ./libds.a;
