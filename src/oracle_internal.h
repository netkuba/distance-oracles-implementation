#ifndef _ORACLE_INTERNAL_H_
#define _ORACLE_INTERNAL_H_

#include "planar.h"
#include <utility>

using std::pair;

void
getAlphaFamily(
        PlanarGraph& g, 
        double alpha, 
        vector< PlanarGraph >& subgs,
        vector< vector<int> >& mappings,
        vector< vector<int> >& parents);

void
subdivide(
        PlanarGraph g,
        vector<int> parent,
        vector< PlanarGraph >& subgs,
        vector< vector<int> >& mappings,
        vector< vector<int> >& parents,
        vector< vector< pair<int, int > > >& paths);

void
getDistances(
        PlanarGraph g,
        int u,
        vector<W> &distances);

#endif
