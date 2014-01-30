#include "ETTree.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <queue>
#include <cstdio>
#include <ctime>
using namespace std;

struct myInt {
    int v;
    myInt(int vv) : v(vv) {}
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

struct myValue {
    int v;
};

struct myMerger {
    int sum;
    myMerger() : sum(0) {}
    void merge_both(myValue* ptr, const myMerger& a, const myMerger& b) {
        sum = a.sum + b.sum;
        if (ptr) sum += ptr->v;
    }
    void merge_left(myValue* ptr, const myMerger& a) {
        sum = a.sum;
        if (ptr) sum += ptr->v;
    }
    void merge_right(myValue* ptr, const myMerger& b) {
        sum = b.sum;
        if (ptr) sum += ptr->v;
    }
    void merge_none(myValue* ptr) {
        sum = 0;
        if (ptr) sum += ptr->v;
    }
};

AVLTree<myInt> mySet, myLeft, myRight;
set<int> stlSet;

const int N = 100;
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
/*
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
*/
/*
    {
        random_shuffle(t, t+N);
        for (int i = 0; i < N; ++i) {
            mySet.insert(t[i]);
        }
        random_shuffle(t, t+N);
        
        cout << "MY: split, merge" << endl;
        start_timing();
        for (int i = 0; i < N; ++i) {
            mySet.split_erase(mySet.find(t[i]), myLeft, myRight);
            myLeft.merge_insert(t[i], myRight);
            swap(myLeft, mySet);
        }
        stop_timing();
    
        for (int i = 0; i < N; ++i) {
            mySet.erase(i);
        }
    }
*/
/*
    {
        random_shuffle(t, t+N);
        for (int i = 0; i < N; ++i) {
            mySet.insert(t[i]);
        }
        random_shuffle(t, t+N);
        
        cout << "MY: split, merge" << endl;
        start_timing();
        for (int i = 0; i < 1; ++i) {
            cout << t[i] << endl;
            mySet.split(mySet.find(t[i]), myLeft, myRight);
            myLeft.merge(myRight);
            swap(myLeft, mySet);
        }
        stop_timing();

        for (int i = 0; i < N; ++i) {
            mySet.erase(i);
        }

    }
*/
    const int n = 20;
    ETForest<myValue, myMerger> forest(n);
    for (int i=0; i<n; ++i){
        forest.vertex_value(i).v = i;
        forest.update(i);
    }

    for (int i=0; i<n; ++i) {
        for (int j=0; j<n; ++j) printf("%d ", forest.vertex_tree_value(j));
        printf("\n");

        int a, b, u, v;
        printf("Podaj a, b, u i v:\n");
        scanf("%d %d %d %d", &a, &b, &u, &v);
        printf("New tree: %d\n", forest.merge(a, b, u, v));
    }

    return 0;
}
