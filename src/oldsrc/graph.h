#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <limits>
#include <memory>
#include <vector>
#include <utility>
using std::shared_ptr;
using std::vector;
using std::pair;
using std::make_pair;
using std::min;
using std::max;

template <class VertexValue, class EdgeValue>
struct Graph {
    struct Vertex;
    struct Edge;

    struct Vertex {
        int ind;
        VertexValue value;

        Vertex(const VertexValue &val) : value(val) {}
        
        Graph* graph;
        vector<Edge*> edges;
    };
    
    struct Edge {
        int ind;
        EdgeValue value;
       
        Edge(const EdgeValue &val, Vertex* uu, Vertex* vv) :
            value(val), u(uu), v(vv) {}

        Vertex *u, *v;

        Vertex* opp(Vertex* w) {
            if (w == u) return v;
            return u;
        }
    };

    shared_ptr< vector<Vertex> > vertices;
    shared_ptr< vector<Edge> > edges;

    Graph() : vertices(new vector<Vertex>), edges(new vector<Edge>) {}

    bool empty() {
        return vs().empty();
    }

    void updateIndices() {
        for (int i=0; i<(int)vs().size(); ++i) vs()[i].ind = i;
        for (int i=0; i<(int)es().size(); ++i) es()[i].ind = i; 
    }

    vector<Vertex>& vs() { return *vertices; }
    vector<Edge>& es() { return *edges; }
};

typedef long double LD;
typedef LD Weight;

typedef int SuperVVal;
struct SuperEVal {
    Weight w;
    SuperEVal(Weight ww) : w(ww) {}
};
typedef Graph<SuperVVal, SuperEVal> G;

struct SubVVal {
    G::Vertex *super;
    SubVVal(G::Vertex *ss) : super(ss) {}
};
struct SubEVal {
    G::Edge *super;
    SubEVal(G::Edge *ss) : super(ss) {}
};
typedef Graph<SubVVal, SubEVal> SubG;

G constructPlanarGraph(
        vector<SuperVVal> vVal,
        vector<SuperEVal> eVal,
        vector< pair<int, int> > e);

pair<Weight, Weight> getStretch(G &g);
vector< SubG > getAlphaFamily(G &g, Weight alpha);


#endif
