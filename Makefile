all: boost_wrapper.o test rotate graph_test planar_test

CXXFLAGS= -std=c++11 -ggdb -Wall

src/ETTree.h: src/AVLTree.h

test: src/test.cpp src/ETTree.h
	g++ src/test.cpp graph.o -o test $(CXXFLAGS)

rotate: src/rotate.cpp src/AVLTree.h
	g++ src/rotate.cpp -o rotate $(CXXFLAGS)

graph.o: src/graph.cpp src/graph.h
	g++ src/graph.cpp -c $(CXXFLAGS)


graph_test: src/graph_test.cpp src/graph.h boost_wrapper.o graph.o
	g++ src/graph_test.cpp graph.o -o graph_test $(CXXFLAGS)

boost_wrapper.o: src/boost_wrapper.cpp src/boost_wrapper.h src/graph.h
	g++ src/boost_wrapper.cpp -c $(CXXFLAGS)

planar.o: src/planar.cpp src/planar.h
	g++ src/planar.cpp -c $(CXXFLAGS)

oracle_internal.o: src/oracle_internal.cpp src/oracle_internal.h
	g++ src/oracle_internal.cpp -c $(CXXFLAGS)

planar_test: src/planar_test.cpp planar.o oracle_internal.o
	g++ src/planar_test.cpp planar.o oracle_internal.o -o planar_test $(CXXFLAGS)

clean:
	rm -f test rotate graph_test boost_wrapper.o
