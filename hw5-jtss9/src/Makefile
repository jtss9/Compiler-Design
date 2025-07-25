CC = g++
LEX = flex
YACC = bison
CFLAGS = -Wall -std=gnu++14 -g -fsanitize=address -fno-omit-frame-pointer
INCLUDE = -Iinclude
ifeq ($(shell uname),Darwin)
LIBS    = -ll
else
LIBS    = -lfl
endif
LIBS    += -ly

SCANNER = scanner
PARSER = parser

ASTDIR = lib/AST/
AST := $(shell find $(ASTDIR) -name '*.cpp')

UTILDIR = lib/util/
UTIL := $(shell find $(UTILDIR) -name '*.cpp')

VISITORDIR = lib/visitor/
VISITOR := $(shell find $(VISITORDIR) -name '*.cpp')

SEMANTICDIR = lib/sema/
SEMANTIC := $(shell find $(SEMANTICDIR) -name '*.cpp')

CODEGENDIR = lib/codegen/
CODEGEN := $(shell find $(CODEGENDIR) -name '*.cpp')

SRC := $(AST) \
       $(UTIL) \
       $(VISITOR) \
       $(SEMANTIC) \
       $(CODEGEN)

EXEC = compiler
OBJS = $(PARSER:=.cpp) \
       $(SCANNER:=.cpp) \
       $(SRC)

# Substitution reference
DEPS := $(OBJS:%.cpp=%.d)
OBJS := $(OBJS:%.cpp=%.o)

all: $(EXEC)

# Static pattern rule
$(SCANNER).cpp: %.cpp: %.l $(PARSER).cpp
	$(LEX) -o $@ $<

$(PARSER).cpp: %.cpp: %.y
	$(YACC) -o $@ --defines=parser.h -v $<

%.o: %.cpp
	$(CC) -o $@ $(CFLAGS) $(INCLUDE) -c -MMD $<

$(EXEC): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS) $(INCLUDE)

clean:
	$(RM) $(DEPS) $(SCANNER:=.cpp) $(PARSER:=.cpp) $(PARSER:=.h) $(PARSER:=.output) $(OBJS) $(EXEC)

-include $(DEPS)
