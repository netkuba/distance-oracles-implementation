#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <utility>
using std::vector;
using std::pair;

template <class VertexValue, class EdgeValue>
struct Graph {
    struct Vertex;
    struct Edge;

    struct Vertex {
        VertexValue value;

        Vertex(const VertexValue &val) : value(val) {}
        
        Graph* graph;
        vector<Edge*> edges;
    };
    
    struct Edge {
        EdgeValue value;
       
        Edge(const EdgeValue &val, Vertex* uu, Vertex* vv) :
            value(val), u(uu), v(vv) {}

        Vertex *u, *v;
    };

    vector<Vertex> vertices;
    vector<Edge> edges;
};

template <class VertexValue, class EdgeValue>
Graph<VertexValue, EdgeValue>
constructPlanarGraph(
        vector<VertexValue> vVal,
        vector<EdgeValue> eVal,
        vector< pair<int, int> > e);

#endif
