#include "AVLTree.h"
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;


struct myInt {
    int v;
    myInt(int v) : v(v) {}
    bool operator<(const myInt& b) const {
        return v < b.v;
    }
    void merge_both(const myInt& a, const myInt& b) {}
    void merge_left(const myInt& a) {}
    void merge_right(const myInt& b) {}
    void merge_none() {}
    friend ostream& operator<<(ostream& os, const myInt& a) {
        os << a.v;
        return os;
    }

};

AVLTree<myInt> mySet, myLeft, myRight;

const int N = 10000000;
int t[N];

int main () {
   
    srand(time(0));

    for (int i = 0; i < N; ++i) {
        t[i] = i;
    }
    random_shuffle(t, t+N);
    for (int i = 0; i < N; ++i) {
        mySet.insert(t[i]);
    }
    random_shuffle(t, t+N);
   
    for (int i = 0; i < N; ++i) {
//        cout << mySet << endl;
        mySet.split(mySet.find(t[i]), myLeft, myRight);
//        cout << t[i] << endl;
//        cout << myLeft << endl;
//        cout << myRight << endl;
        myLeft.merge(t[i], myRight);
        swap(mySet, myLeft);
    }
        

    return 0;
}
