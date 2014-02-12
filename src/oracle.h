#ifndef _ORACLE_H_
#define _ORACLE_H_

#include "planar.h"
#include "oracle_internal.h"

class PlanarOracle {
private:
public:
    PlanarGraph graph;
    vector< pair< PlanarGraph, vector<int> > > pieces;
    vector< vector<int> > portals;
    vector< vector< pair< int, int > > > clusters;

    PlanarOracle(
            int n,
            vector< pair< int, int > > edges, 
            vector< W > weights,
            W eps);

    void merge(int u, int v);
    W distance(int u, int v);
};

#endif
