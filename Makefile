CFLAGS_DEBUG = -g -fprofile-arcs -ftest-coverage
CFLAGS_RELEASE = -O3 -DNDEBUG
CFLAGS = -std=c99 -pedantic -Wall -Wno-override-init-side-effects -Wno-unused-function -Werror

IMPL_SRCS = ./vec/vec.c ./mem/pool/pool.c ./map/map.c ./set/set.c
IMPL_OBJS = $(patsubst ./%.c,./%.o,$(IMPL_SRCS))

TEST_SRCS = ./test/test_arr.c ./test/test_vec.c ./test/test_map.c ./test/test_mem_pool.c ./test/test_set.c
TEST_OBJS = $(patsubst ./%.c,./%.o,$(TEST_SRCS))

libds.a: $(IMPL_OBJS)
	ar -rc libds.a $(IMPL_OBJS)

$(IMPL_OBJS): ./%.o: ./%.c
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) -c -o $@ $<

$(TEST_OBJS): test/%.o: test/%.c
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -c -o $@ $<

test/test: $(TEST_SRCS) $(IMPL_SRCS) clean-coverage
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -o test/test test/test.c $(TEST_SRCS) test/munit/munit.c -lm

.PHONY: test
test: test/test
	./test/test

.PHONY: coverage
coverage: test
	gcovr -s \
		--exclude-lines-by-pattern '\s*assert\(.*' \
		--exclude-unreachable-branches \
		--html-details ./coverage/coverage.html \
		--exclude test/test.c \
		--exclude test/munit/munit.c \
		$(foreach src,$(patsubst ./test/%.c,test/%.c,$(TEST_SRCS)),--exclude $(src))
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

.PHONY: run
run: test
