#ifndef _ORACLE_H_
#define _ORACLE_H_

#include "planar.h"
#include "oracle_internal.h"

#include <map>
#include <unordered_map>
#include <set>
#include <cmath>
using std::map;
using std::unordered_map;
using std::set;
using std::min;
using std::max;
using std::sqrt;

class PlanarOracle {
protected:

    int ro;

    struct Piece {
        PlanarGraph graph;
        vector<int> mapping;

        Piece(const PlanarGraph& gg, 
                const vector<int>& mm) :
            graph(gg), mapping(mm) {}
    };

    struct Vertex {
        map<int, W> to_portal;
        vector<int> direct_pieces;
    };

    virtual
    bool isOfColor(int v, int c) = 0;

    virtual
    bool isActive(int v) = 0;

    virtual
    void processLeaf(
            int i,
            const PlanarGraph& pg,
            const vector<int>& mapping,
            const vector<bool>& source) = 0;

    virtual
    void processPortals(
            int i,
            const PlanarGraph& pg,
            const vector<int>& mapping,
            const vector<int>& portal,
            const vector<bool>& source) = 0;

    void initialize(
            int n,
            const vector< pair< int, int > >& edges, 
            const vector< W >& weights,
            W eps);

    PlanarGraph graph;
    vector< Piece > pieces;
    vector< Vertex > vertices;

public:
    PlanarOracle() {}
    PlanarOracle(
            int n,
            const vector< pair< int, int > >& edges, 
            const vector< W >& weights,
            W eps) {
        initialize(n, edges, weights, eps);
    }
};

#endif
