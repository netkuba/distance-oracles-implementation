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

public:
    PlanarGraph graph;
    vector< pair< PlanarGraph, vector<int> > > pieces; // with mapping
    vector< Portal > portals; // for each graph list of portals and their indices
    vector< map<int, W> > vertex_to_portal;
    vector< Label > labels;
   
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
