#ifndef _INCREMENTAL_ORACLE_H_
#define _INCREMENTAL_ORACLE_H_

#include "oracle.h"

class IncrementalPlanarOracle : public PlanarOracle {
    
    struct Label {
        bool active = false;
        vector< pair<int, int> > L; // pair (v, p)
    };

    struct Portal {
        int p, v;
        unordered_map<int, W> N;
        set< pair<W, int> > H;
        
        Portal(int pp, int vv) : p(pp), v(vv) {}
    };

    struct FindUnion {
        vector<int> p;
        
        FindUnion(int n) : p(n) {
            for (int i=0; i<n; ++i) p[i] = i;
        }

        int find(int v) {
            if (p[v] == v) return v;
            return v = find(p[v]);
        }

        void unionn(int v, int u) {
            p[find(u)] = find(v);
        }

    };
    
    virtual
    bool isOfColor(int v, int c) {
        return fu.find(v) == c;
    }

    virtual
    bool isActive(int v) {
        return labels[fu.find(v)].active;
    }
    
    virtual
    void processLeaf(
            int i,
            const PlanarGraph& pg,
            const vector<int>& mapping,
            const vector<bool>& source);

    virtual
    void processPortals(
            int i,
            const PlanarGraph& pg,
            const vector<int>& mapping,
            const vector<int>& portal,
            const vector<bool>& source);
    
    vector< Label > labels;
    vector< Portal > portals;
    FindUnion fu;

public:
    IncrementalPlanarOracle() : fu(0) {}
    IncrementalPlanarOracle(
            int n,
            const vector< pair< int, int > >& edges, 
            const vector< W >& weights,
            W eps) : labels(n), fu(n) {
        initialize(n, edges, weights, eps);
    }

    int merge(int l1, int l2);
    
    void activate(int l);
    
    W distance_to_color(int v, int l);
    W distance_to_closest(int v);
};

#endif
