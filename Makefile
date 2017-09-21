LD=g++
LDFLAGS=

CXX=g++
CXXFLAGS=-std=c++17 -I . -Wall -Werror -pedantic

.PHONY: all test clean mrproper

OBJECTS=test.o
OBJECTS_DEBUG=test-debug.o

TARGET=test.out
TARGET_DEBUG=test-debug.out

all: ${TARGET} ${TARGET_DEBUG}

test: ${TARGET}
	./${TARGET}

${TARGET}: ${OBJECTS}
	${LD} ${LDFLAGS} $^ -o $@

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

${TARGET_DEBUG}: ${OBJECTS_DEBUG}
	${LD} ${LDFLAGS} $^ -o $@ -g

%-debug.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@ -g

test.o: vec_list

clean:
	@rm -f ${OBJECTS}

mrproper:
	@rm -f ${TARGET} ${TARGET_DEBUG}
