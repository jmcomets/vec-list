LD=g++
CXX=g++

CXXFLAGS=-std=c++17 -g

.PHONY: clean mrproper

OBJECTS=main.o

TARGET=main

${TARGET}: ${OBJECTS}
	${LD} ${LDFLAGS} $^ -o $@

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

main.o: llist.h

clean:
	@rm -f ${OBJECTS}

mrproper:
	@rm -f ${TARGET}
