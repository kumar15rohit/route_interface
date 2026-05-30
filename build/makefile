SRC=$(wildcard ../src/*.c)
INC_DIR=../include
SAMPLE_CONFIG=$(wildcard ../sample/*.json)
BIN_DIR=bin
UT=../ut

CC=gcc
CFLAGS=-I$(INC_DIR) -Wall -Wextra -g3

TARGET=$(BIN_DIR)/router_demo

all:
	mkdir -p $(BIN_DIR)
	cp -f $(SAMPLE_CONFIG) $(BIN_DIR)/
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(BIN_DIR)/*

test_router:
	$(CC) $(CFLAGS) \
	../src/interface.c \
	../ut/ut_router.c \
	../src/route_table_manager.c \
	../src/lpm_engine.c \
	../src/common.c \
	../src/print_utils.c \
	-o $(BIN_DIR)/test_router

test_cli:
	$(CC) $(CFLAGS) \
	../src/interface.c \
	../src/route_table_manager.c \
	../src/lpm_engine.c \
	../src/common.c \
	../src/cli.c \
	../src/print_utils.c \
	../ut/ut_cli.c \
	-o $(BIN_DIR)/test_cli

test_all: test_router test_cli
