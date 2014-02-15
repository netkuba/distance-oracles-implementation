#ifndef _ORACLE_H_
#define _ORACLE_H_

#include "planar.h"
#include "oracle_internal.h"

#include <unordered_map>
using std::unordered_map;

class PlanarOracle {
private:

    struct Portal {
        int p, v;
        unordered_map<int, W> to_vertex;
        unordered_map<int, W> to_color;
        
        Portal(int pp, int vv) : p(pp), v(vv) {}
    };

public:
    PlanarGraph graph;
    vector< pair< PlanarGraph, vector<int> > > pieces; // with mapping
    vector< Portal > portals; // for each graph list of portals and their indices
    vector< unordered_map<int, W> > vertex_to_portal;
   
    PlanarOracle(
            int n,
            vector< pair< int, int > > edges, 
            vector< W > weights,
            W eps);

    void merge(int u, int v);

    W exact_to_vertex(int v, int u);
    W distance_to_vertex(int v, int u);
    W distance_to_color(int v, int u);
    W distance_to_closest(int v);
};

#endif
