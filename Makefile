vec_test: vec_test.c vec.h
	c89 -o vec_test vec_test.c

.PHONY: test
test: vec_test
	valgrind ./vec_test | less
