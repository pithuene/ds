CFLAGS_DEBUG = -g -fprofile-arcs -ftest-coverage
CFLAGS = -std=c99 -pedantic -Wall -Wno-override-init-side-effects -Wno-unused-function -Werror $(CFLAGS_DEBUG)

IMPL_SRCS = ./vec/vec.c ./mem/pool/pool.c
IMPL_OBJS = $(patsubst ./%.c,./%.o,$(IMPL_SRCS))

TEST_SRCS = ./test/test_arr.c ./test/test_vec.c ./test/test_mem_pool.c
TEST_OBJS = $(patsubst ./%.c,./%.o,$(TEST_SRCS))

$(IMPL_OBJS): ./%.o: ./%.c
	$(CC) $(CFLAGS) -c -o $@ $<

libds.a: $(IMPL_OBJS)
	ar -rc libds.a $(IMPL_OBJS)

$(TEST_OBJS): test/%.o: test/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

test/test: $(TEST_OBJS) libds.a clean-coverage
	$(CC) $(CFLAGS_DEBUG) -lm -o test/test test/test.c $(TEST_OBJS) libds.a test/munit/munit.c

.PHONY: test
test: test/test
	./test/test

.PHONY: coverage
coverage: test
	gcovr -s --html-details ./coverage/coverage.html --exclude test/test.c --exclude test/munit/munit.c --exclude test/test_arr.c --exclude test/test_vec.c --exclude test/test_mem_pool.c
	xdg-open ./coverage/coverage.html

# Remove all previous coverage data
.PHONY: clean-coverage
clean-coverage:
	find . -name '*.gcda' -delete;
	rm -f ./coverage/*

.PHONY: clean
clean: clean-coverage
	find . -name '*.gcno' -delete
	find . -name '*.o' -delete;
	rm -f ./test/test
	rm -f ./libds.a
