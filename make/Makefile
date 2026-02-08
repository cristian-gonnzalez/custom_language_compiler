CC=gcc
COMPILER=../target/compiler

BISON_FILE=../src/syntax/compiler.y
BISON_OUT=../build/syntax
BISON_SRC=$(BISON_OUT)/compiler.tab.c
BISON_HDR=$(BISON_OUT)/compiler.tab.h

FLEX_FILE=../src/lex/compiler.l
FLEX_OUT=../build/lex
FLEX_SRC=$(FLEX_OUT)/lex.yy.c

INCLUDES=-I../src/compiler/ast -I../src/compiler/codegen \
          -I../src/compiler/core -I../src/compiler/semantic -I$(BISON_OUT)

SRC_FILES=../src/compiler/ast/*.c ../src/compiler/codegen/*.c \
          ../src/compiler/core/*.c ../src/compiler/semantic/*.c \
          $(BISON_SRC) \
          $(FLEX_SRC)

.PHONY: all clean

all: $(COMPILER)

$(BISON_SRC) $(BISON_HDR): $(BISON_FILE)
	mkdir -p $(BISON_OUT)
	bison -d -o $(BISON_SRC) $(BISON_FILE)

$(FLEX_SRC): $(FLEX_FILE)
	mkdir -p $(FLEX_OUT)
	flex -o $(FLEX_SRC) $(FLEX_FILE)

$(COMPILER): $(SRC_FILES)
	mkdir -p ../target
	$(CC) -std=c99 -D_POSIX_C_SOURCE=200809L -g $(SRC_FILES) $(INCLUDES) -o $(COMPILER)

clean:
	rm -rf ../build $(COMPILER)




