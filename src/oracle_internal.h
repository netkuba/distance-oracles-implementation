#ifndef _ORACLE_INTERNAL_H_
#define _ORACLE_INTERNAL_H_

#include "planar.h"
#include <utility>

using std::pair;

void
getDistances(
        const PlanarGraph& g,
        int u,
        vector<W> &distances);

void
getAlphaFamily(
        const PlanarGraph& g, 
        double alpha, 
        vector< PlanarGraph >& subgs,
        vector< vector<int> >& mappings,
        vector< vector<int> >& parents,
        vector< vector<bool> >& sources);

void
subdivide(
        PlanarGraph& g,
        const vector<int>& parent,
        vector< PlanarGraph >& subgs,
        vector< vector<int> >& mappings,
        vector< vector<int> >& parents,
        vector< vector< pair<int, int > > >& paths);

#endif
