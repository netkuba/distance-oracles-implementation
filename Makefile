all: test_ merge_

test_: src/test.cpp src/AVLTree.h
	g++ src/test.cpp -o test -std=c++11 -ggdb

merge_: src/merge.cpp src/AVLTree.h
	g++ src/merge.cpp -o merge -std=c++11 -ggdb
