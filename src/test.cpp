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

struct myMergerList {
    vector<int> list;
    myMergerList() {}
    void merge_both(myValue* ptr, const myMergerList& a, const myMergerList& b) {
        list.clear();
        list.insert(list.end(), a.list.begin(), a.list.end());
        list.insert(list.end(), b.list.begin(), b.list.end());
        if (ptr) list.push_back(ptr->v);
        assert((list.size() >= 0) && (list.size() < 100));
    }
    void merge_left(myValue* ptr, const myMergerList& a) {
        list.clear();
        list.insert(list.end(), a.list.begin(), a.list.end());
        if (ptr) list.push_back(ptr->v);
        assert((list.size() >= 0) && (list.size() < 100));
    }
    void merge_right(myValue* ptr, const myMergerList& b) {
        list.clear();
        list.insert(list.end(), b.list.begin(), b.list.end());
        if (ptr) list.push_back(ptr->v);
        assert((list.size() >= 0) && (list.size() < 100));
    }
    void merge_none(myValue* ptr) {
        list.clear();
        if (ptr) list.push_back(ptr->v);
        assert((list.size() >= 0) && (list.size() < 100));
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
    cout << "Time: " << (clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC) << endl;
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
*/
/*
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
        for (int i = 0; i < N; ++i) {
//            cout << t[i] << endl;
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
/*
    const int n = 20;
    ETForest<myValue, myMerger> forest(n);
    for (int i=0; i<n; ++i){
        forest.vertex_value(i).v = i;
        forest.update(i);
    }

    for (int i=0; i<n; ++i) {
        for (int j=0; j<n; ++j) cout << forest.vertex_tree_value(j) << " ";
        cout << endl;
        for (int j=0; j<n; ++j) {
//            cout << forest.values[j] << endl;
        }
        printf("\n");

        int a, b, u, v;
        printf("Podaj a, b, u i v:\n");
        scanf("%d %d %d %d", &a, &b, &u, &v);
        printf("New tree: %d\n", forest.merge(a, b, u, v));
    }
*/
 
    const int n = 20;
    ETForest<myValue, myMergerList> forest(n);
    for (int i=0; i<n; ++i) {
        forest.vertex_value(i).v = i;
        forest.update(i);
    }

    while (true) {
        for (int i=0; i<n; ++i) {
            if (forest.vertex_tree_number(i) == i) {
                cout << i << ": ";
                vector<int> *list = &forest.vertex_tree_value(i).list;
                for (int j=0; j<(int)list->size(); ++j) {
                    cout << (*list)[j] << " ";
                }
                cout << endl;
            }
        }
        string s;
        cin >> s;
        if (s == "ins") {
            int u, v;
            cin >> u >> v;
            forest.merge(forest.vertex_tree_number(u), forest.vertex_tree_number(v), u, v);
        } else if (s == "del") {
            int u, v;
            cin >> u >> v;
            pair<int, int> pp = forest.split(forest.vertex_tree_number(u), u, v);
            cout << pp.first << " " << pp.second << endl;
        } else if (s == "exit") {
            break;
        }
        cout << "Again..." << endl;
    }
    return 0;
}
