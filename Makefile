# 3DViewer-v1.0
PROJECT_NAME = 3DViewer

# Creating directories for objects, tests, binary, coverage files.
TESTS_DIR = ./tests
REPORT_DIR = ./report
GCOV_DIR = ./gcov
DOC_DIR = ./doc
BUILD_DIR = ./$(PROJECT_NAME)-build
ARCHIVE_DIR = ./$(PROJECT_NAME)-v1.0
INSTALL_DIR = ./Home/Programs/$(PROJECT_NAME)-v1.0

# Location of the source file

KERNEL_PATH = ./kernel
APP_PATH = ./app

# Main list flags.
CC ?= gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

# Separate options for MacOS.
OS := $(shell uname -s)
ifeq ($(OS), Darwin)
	OPEN_CMD = open
	LCHECK =
	PROJECT_NAME +=.app
endif

# Separate options for Linux.
ifeq ($(OS), Linux)
	OPEN_CMD = xdg-open
	LCHECK = `pkg-config --cflags --libs check`
endif

# Coverage GCOV.
FGCOV = -fprofile-arcs -ftest-coverage

SOURCE = $(wildcard $(KERNEL_PATH)/*.c)
OBJECTS = $(addprefix $(GCOV_DIR)/, $(notdir $(SOURCE:%.c=%.o)))

# Tests check.h
TESTS = $(TESTS_DIR)/000.include
TESTS += $(wildcard $(TESTS_DIR)/*.check)

# Targets

all: build

install: build
	mkdir -vp $(INSTALL_DIR)
	cd $(BUILD_DIR)/app && cp -r $(PROJECT_NAME) ../../$(INSTALL_DIR)

uninstall:
	cd ./Home/Programs && rm -rf ./$(PROJECT_NAME)-v1.0

build: clean_checkgen
	cmake -B ./$(PROJECT_NAME)-build -DCMAKE_BUILD_TYPE=Debug -DPROJECT_TESTING=ON -DPROJECT_COVERAGE=ON -DPROJECT_DOC=OFF
	cmake --build ./$(PROJECT_NAME)-build

run: build
	-./$(PROJECT_NAME)-build/app/$(PROJECT_NAME)

dvi:
	doxygen Doxyfile
	open ./doc/html/index.html

dist: clean
	mkdir -vp $(ARCHIVE_DIR)
	cp -r Makefile *.txt *.md doxyfile ./tests ./kernel ./app ./submodules $(ARCHIVE_DIR)
	tar -cvzf $(PROJECT_NAME)-v1.0.tar $(ARCHIVE_DIR)
	rm -rf $(ARCHIVE_DIR)

tests: build
	-./$(PROJECT_NAME)-build/tests/$(PROJECT_NAME)-tests

clean_checkgen:
	rm -rf $(BUILD_DIR)/tests/check-tests.c

tests-make:
	checkmk $(TESTS) > $(TESTS_DIR)/tests.c
	clang -g -o $(TESTS_DIR)/tests_$(PROJECT_NAME) $(TESTS_DIR)/tests.c kernel/dataset.c kernel/transform.c kernel/utils.c $(LCHECK)

$(GCOV_DIR)/%.o: $(KERNEL_PATH)/%.c
	$(CC) $(FGCOV) -c -MD $< -o $@

gcov_report: $(OBJECTS)
	mkdir -vp $(GCOV_DIR)
	
	$(CC) $(BUILD_DIR)/tests/check-tests.c -c -o $(GCOV_DIR)/check-tests.c.o $(LCHECK)
	$(CC) $(OBJECTS) $(GCOV_DIR)/check-tests.c.o $(FGCOV) $(LCHECK) -o $(GCOV_DIR)/$(PROJECT_NAME)-cov-bin
	
	-./$(GCOV_DIR)/$(PROJECT_NAME)-cov-bin
	
	lcov -t "$(PROJECT_NAME)-cov-bin" -o $(GCOV_DIR)/$(PROJECT_NAME)-cov-bin.info -c -d $(GCOV_DIR)
	genhtml -o $(REPORT_DIR) $(GCOV_DIR)/$(PROJECT_NAME)-cov-bin.info
	
	-$(OPEN_CMD) $(REPORT_DIR)/index-sort-f.html

style_fix:
	cp ../materials/linters/.clang-format $(KERNEL_PATH)/
	cp ../materials/linters/.clang-format $(APP_PATH)/

	clang-format -i $(APP_PATH)/*.cpp $(APP_PATH)/*.h 
	clang-format -i $(KERNEL_PATH)/*.c $(KERNEL_PATH)/*.h
	rm -rf $(APP_PATH)/.clang-format $(KERNEL_PATH)/.clang-format

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes --verbose $(BUILD_DIR)/tests/$(PROJECT_NAME)-tests

leaks: build
	leaks -atExit -- $(BUILD_DIR)/tests/$(PROJECT_NAME)-tests

check:
#   cppcheck -q --enable=all --inconclusive --platform=unix32 --std=c11 --template='{id} {file}:{line} {message}' --suppress=missingIncludeSystem $(SOURCE) *.h
	@echo "----- cppcheck --------"	
	cppcheck -q --enable=all --inconclusive --std=c11 --template='{id} {file}:{line} {message}' --suppress=missingIncludeSystem $(KERNEL_PATH) *.h
	cppcheck -q --enable=all --inconclusive --std=c11 --template='{id} {file}:{line} {message}' --suppress=missingIncludeSystem $(APP_PATH) *.h
	@echo "----- style check -----"
	cp ../materials/linters/.clang-format $(KERNEL_PATH)/
	cp ../materials/linters/.clang-format $(APP_PATH)/
	clang-format -n --style=Google $(APP_PATH)/*.cpp $(APP_PATH)/*.h 
	clang-format -n --style=Google $(KERNEL_PATH)/*.c $(KERNEL_PATH)/*.h
	rm -rf $(APP_PATH)/.clang-format $(KERNEL_PATH)/.clang-format
	
clean:
	rm -rf ./$(PROJECT_NAME)-build ./build-src-Desktop-Debug
	rm -rf $(DOC_DIR) ./*.user 
	rm -rf ./images
	rm -rf $(TESTS_DIR)/check-tests.c $(TESTS_DIR)/tests.c $(TESTS_DIR)/tests_$(PROJECT_NAME)
	rm -rf $(REPORT_DIR) $(GCOV_DIR) 
	rm -rf $(PROJECT_NAME)-v1.0.tar
	rm -rf ./Home

rebuild: build

.PHONY: all clean