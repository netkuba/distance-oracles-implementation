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
    long long c;

    myInt(int v) : v(v), c(0) {}
    bool operator<(const myInt& b) const {
        return v < b.v;
    }
    void merge_both(const myInt& a, const myInt& b) { ++c; }
    void merge_left(const myInt& a) { ++c; }
    void merge_right(const myInt& b) { ++c;}
    void merge_none() { ++c; }
    friend ostream& operator<<(ostream& os, const myInt& a) {
        os << a.c;
        return os;
    }

};

AVLTree<myInt> mySet;
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
    
        long long sum = 0;
        for (auto it = mySet.begin(); it != mySet.end(); ++it) {
            sum += (*it).c;
        }

        cout << (long double)sum / N << endl;

        for (int i = 0; i < N; ++i) {
            mySet.erase(i);
        }
    }

    return 0;
}
