#include "boost_wrapper.h"

template
Graph<int, int>
constructPlanarGraph(
        vector<int> vVal,
        vector<int> eVal,
        vector< pair<int, int> > e);

template
Graph<SuperVVal, SuperEVal>
constructPlanarGraph(
        vector<SuperVVal> vVal,
        vector<SuperEVal> eVal,
        vector< pair<int, int> > e);
