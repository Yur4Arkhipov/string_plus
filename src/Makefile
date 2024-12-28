CC=gcc
CC_FLAGS=-c -std=c11 -Wall -Werror -Wextra
BUILD_NAME=s21_string
GCOV=-fprofile-arcs -ftest-coverage

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

# for testing string functions
TEST_FLAGS=-lcheck -lsubunit -lrt -lm -lpthread
TEST_SRC=${mkfile_path}/tests/test_s21_string_functions.c
TEST_OBJ=$(TEST_SRC:.c=.o)
TEST_TARGET = test_s21_string_functions

all: build test gcov_report

build: s21_string.a

tests/${TEST_TARGET}.c: tests/$(TEST_TARGET).check
	checkmk tests/$(TEST_TARGET).check > tests/${TEST_TARGET}.c 

#запуск теста
test: tests/${TEST_TARGET}.c s21_string.a
	${CC} tests/$(TEST_TARGET).c s21_string.a $(TEST_FLAGS) -o tests/${TEST_TARGET}
	./tests/$(TEST_TARGET)

rebuild: clean build

s21_string.a: s21_string.o s21_string.h s21_sprintf.o
	ar rcs s21_string.a s21_string.o s21_sprintf.o 
	ranlib s21_string.a

s21_string.o: s21_string.c
	${CC} ${CC_FLAGS} ${BUILD_NAME}.c

s21_sprintf.o: s21_sprintf.c
	${CC} ${CC_FLAGS} s21_sprintf.c

gcov_report: s21_string.c s21_sprintf.c tests/$(TEST_TARGET).c
	${CC} --coverage tests/$(TEST_TARGET).c s21_string.c s21_sprintf.c ${TEST_FLAGS} -o tests/test_report
	./tests/test_report
	lcov --directory . --capture -o coverage.info
	genhtml --output-directory report --legend coverage.info
	rm ./tests/*.gcda
	rm ./tests/*.gcno
	rm coverage.info

clean:
	-rm -rf ./*.o
	-rm ./*.a
	-rm ./tests/test_s21_string_functions
	-rm ./tests/test_report
	-rm tests/test_s21_string_functions.c
	-rm -rf report