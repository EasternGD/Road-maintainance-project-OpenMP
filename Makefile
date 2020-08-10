all: main.exe test
	CC=g++
	CFLAGS=-O2 -Wall -std=c++11 -fopenmp

%.o: src/%.cpp inc/%.h
	$(CC) $(CFLAGS) -c $< -o $@

main.exe: main.cpp function.o
	$(CC) $(CFLAGS) $< -o $@ function.o
	test: main.exe
	@./main.exe data/testfile.csv testfile.json 0.06006 0.31459 0.88428 0.71797 0.61730 50000000

small: main.exe
	./main.exe data/01.csv 01.json 0.8 0.1 0.8 0.5 0.8 15000000

clean:
	rm *.exe *.o src/*.o *.json *.out
