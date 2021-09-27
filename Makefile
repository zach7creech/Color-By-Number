ALL = bin/color_by_number

CC = g++

INCLUDE = -Iinclude

CFLAGS = -std=c++11

all: $(ALL)

clean:
	rm obj/* bin/color_by_number

obj/disjoint_set.o: src/disjoint_set.cpp include/disjoint_set.hpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o obj/disjoint_set.o src/disjoint_set.cpp

obj/color_by_number.o: src/color_by_number.cpp include/disjoint_set.hpp include/color_by_number.hpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o obj/color_by_number.o src/color_by_number.cpp

bin/color_by_number: obj/color_by_number.o obj/disjoint_set.o
	$(CC) $(INCLUDE) $(CFLAGS) -o bin/color_by_number obj/color_by_number.o obj/disjoint_set.o

$(shell sh color-by-number.sh examples/bold_and_brash.jpg examples/pdfs/bold_and_brash.pdf robust large)
$(shell sh color-by-number.sh examples/gura.jpg examples/pdfs/gura.pdf robust large)
$(shell sh color-by-number.sh examples/link.jpg examples/pdfs/link.pdf robust large)
$(shell sh color-by-number.sh examples/megumin.jpg examples/pdfs/megumin.pdf robust large)
$(shell sh color-by-number.sh examples/mountain.jpg examples/pdfs/mountain.pdf robust large -L)
$(shell sh color-by-number.sh examples/oni.jpg examples/pdfs/oni.pdf robust large -L)
$(shell sh color-by-number.sh examples/starry_night.jpg examples/pdfs/starry_night.pdf robust large -L)
$(shell sh color-by-number.sh examples/starry_night.jpg examples/pdfs/starry_night_small.pdf robust small -L)
