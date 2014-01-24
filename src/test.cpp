#include "AVLTree.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <queue>
#include <cstdio>
#include <ctime>
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
set<int> stlSet;

const int N = 1000000;
int t[N];

clock_t start_time;
void start_timing() {
    start_time = clock();
}

void stop_timing() {
    cout << (clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC) << endl;
}

int main() {
    
    for (int i = 0; i < N; ++i) {
        t[i] = i;
    }

    {
        random_shuffle(t, t+N);
        for (int i = 0; i < N; ++i) {
            stlSet.insert(t[i]);
        }
        random_shuffle(t, t+N);
        
        cout << "STL: insert, erase" << endl;
        start_timing();
        for (int i = 0; i < N; ++i) {
            stlSet.erase(t[i]);
            stlSet.insert(t[i]);
        }
        stop_timing();
        
        for (int i = 0; i < N; ++i) {
            stlSet.erase(i);
        }
    }

    {
        random_shuffle(t, t+N);
        for (int i = 0; i < N; ++i) {
            mySet.insert(t[i]);
        }
        random_shuffle(t, t+N);
        
        cout << "MY: insert, erase" << endl;
        start_timing();
        for (int i = 0; i < N; ++i) {
            mySet.erase(t[i]);
            mySet.insert(t[i]);
        }
        stop_timing();
        
        for (int i = 0; i < N; ++i) {
            mySet.erase(i);
        }
    }

    {
        random_shuffle(t, t+N);
        for (int i = 0; i < N; ++i) {
            mySet.insert(t[i]);
        }
        random_shuffle(t, t+N);
        
        cout << "MY: split, merge" << endl;
        start_timing();
        for (int i = 0; i < N; ++i) {
            mySet.split(mySet.find(t[i]), myLeft, myRight);
            myLeft.merge(t[i], myRight);
            swap(myLeft, mySet);
        }
        stop_timing();
        
        for (int i = 0; i < N; ++i) {
            mySet.erase(i);
        }
    }

    return 0;
}
