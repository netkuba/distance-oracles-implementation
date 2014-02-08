all: boost_wrapper.o test rotate graph_test

src/ETTree.h: src/AVLTree.h

test: src/test.cpp src/ETTree.h
	g++ src/test.cpp -o test -std=c++11 -ggdb

rotate: src/rotate.cpp src/AVLTree.h
	g++ src/rotate.cpp -o rotate -std=c++11 -ggdb

graph_test: src/graph_test.cpp src/graph.h boost_wrapper.o
	g++ src/graph_test.cpp boost_wrapper.o -o graph_test -std=c++11 -ggdb

boost_wrapper.o: src/boost_wrapper.cpp src/boost_wrapper.h src/graph.h
	g++ src/boost_wrapper.cpp -c -std=c++11 -ggdb
