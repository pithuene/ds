types_test: types_test.c types.h
	c89 -o types_test types_test.c

vec_test: vec_test.c vec.h
	c89 -o vec_test vec_test.c

map_test: map_test.c map.h
	c89 -o map_test map_test.c

.PHONY: clean
clean:
	rm -f ./vec_test; \
	rm -f ./linalloc_test

.PHONY: test_types
test_types: types_test
	valgrind --leak-check=full ./types_test

.PHONY: test_vec
test_vec: vec_test
	valgrind --leak-check=full ./vec_test

.PHONY: test_linalloc
test_linalloc: linalloc_test
	valgrind --leak-check=full ./linalloc_test

.PHONY: test_map
test_map: map_test
	valgrind --leak-check=full ./map_test

.PHONY: test
test: test_vec test_linalloc

