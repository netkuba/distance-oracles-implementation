all: test rotate

test: src/test.cpp src/AVLTree.h src/ETTree.h
	g++ src/test.cpp -o test -std=c++11 -ggdb

rotate: src/rotate.cpp src/AVLTree.h
	g++ src/rotate.cpp -o rotate -std=c++11 -ggdb
