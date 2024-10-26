
SOURCES=Song.cpp Database.cpp cmdapp.cpp
OUTPUT_FILE=songbook
O_FILES=
H_FILES=./json/json.hpp ./argparse/argparse.hpp

DEBUG_FLAG=


.PHONY: all
all: compile link run

.PHONY: compile-lib
compile-lib:
	g++ -c $(H_FILES)

.PHONY: build
build: compile-lib compile link

.PHONY: debug
debug: compile-with-debug link
	gdb ./$(OUTPUT_FILE)

.PHONY: run
run:
	@./$(OUTPUT_FILE)

.PHONY: compile-with-debug
compile-with-debug:
	g++ -g -c $(H_FILES) $(SOURCES)

compile:
	g++ $(DEBUG_FLAG) -c $(SOURCES)

.PHONY: clean
clean:
	rm *.o
	rm *.gch
	rm ./$(OUTPUT_FILE)


# TODO
link:
	g++ Song.o Database.o cmdapp.o -o $(OUTPUT_FILE)
	
