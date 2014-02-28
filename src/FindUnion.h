#ifndef _FINDUNION_H_
#define _FINDUNION_H_

#include <vector>
using std::vector;

struct FindUnion {
    vector<int> p;
    
    FindUnion(int n) : p(n) {
        for (int i=0; i<n; ++i) p[i] = i;
    }

    int find(int v) {
        if (p[v] == v) return v;
        return v = find(p[v]);
    }

    int unionn(int v, int u) {
        return p[find(u)] = find(v);
    }

};

#endif
