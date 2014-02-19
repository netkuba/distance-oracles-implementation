#ifndef _ORACLE_H_
#define _ORACLE_H_

#include "planar.h"
#include "oracle_internal.h"

#include <map>
#include <set>
using std::map;
using std::set;

class PlanarOracle {
private:

    static const int ro = 500;

    struct Portal {
        int p, v;
        map<int, W> N;
        set< pair<W, int> > H;
        
        Portal(int pp, int vv) : p(pp), v(vv) {}
    };

    struct Label {
        bool active = false;
        vector< pair<int, int> > L; // pair (v, p)
    };

    struct Vertex {
        map<int, W> to_portal;
        vector<int> direct_pieces;
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

    bool isOfColor(int v, int c) {
        return fu.find(v) == c;
    }

    void processLeaf(
            int i,
            PlanarGraph pg,
            vector<int>& mapping);

    void processPortals(
            int i,
            PlanarGraph& pg,
            vector<int>& mapping,
            vector<int>& portal);

public:
    PlanarGraph graph;
    vector< pair< PlanarGraph, vector<int> > > pieces; // with mapping
    vector< Portal > portals; // for each graph list of portals and their indices
    vector< Vertex > vertices;
    vector< Label > labels;
    FindUnion fu;


    PlanarOracle(
            int n,
            vector< pair< int, int > > edges, 
            vector< W > weights,
            W eps);


    int merge(int v, int u);
    void activate(int v);
    W distance_to_color(int v, int u);
    W distance_to_closest(int v);

    W exact_to_vertex(int v, int u);
};

#endif
