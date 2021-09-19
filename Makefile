ALL = bin/color_by_number

CC = g++

INCLUDE = -Iinclude

CFLAGS = -std=c++11

all: $(ALL)

clean:
	rm obj/* bin/color_by_number

obj/disjoint_set.o: src/disjoint_set.cpp include/disjoint_set.hpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o obj/disjoint_set.o src/disjoint_set.cpp

obj/color_by_number.o: src/color_by_number.cpp include/disjoint_set.hpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o obj/color_by_number.o src/color_by_number.cpp

bin/color_by_number: obj/color_by_number.o obj/disjoint_set.o
	$(CC) $(INCLUDE) $(CFLAGS) -o bin/color_by_number obj/color_by_number.o obj/disjoint_set.o
