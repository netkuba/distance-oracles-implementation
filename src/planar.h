#ifndef _PLANAR_H_
#define _PLANAR_H_

#include <vector>
#include <utility>
#include <limits>

using std::vector;

typedef double W;
const W infinity = std::numeric_limits<W>::infinity();

class PlanarGraph {
public:
    struct Edge {
        Edge(int uu, int vv, W ww) : 
            ind(-1), u(uu), v(vv), w(ww) {}
        Edge(int uu, int vv) : Edge(uu, vv, infinity) {}

        int ind;
        int u, v;
        int uNext, vNext;
        W w;
    };

    struct Vertex {
        Vertex() : 
            ind(-1) {}

        int ind;
        vector<int> edges;
    };

    vector<Vertex> vertices;
    vector<Edge> edges;

    PlanarGraph() {}
    PlanarGraph(int n) : vertices(n) {}

    vector<Vertex>& vs() {
        return vertices;
    }

    vector<Edge>& es() {
        return edges;
    }
    
    int add_edge(int u, int v, W w = infinity) {
        int res = es().size();
        es().push_back(Edge(u, v, w));
        vs()[u].edges.push_back(res);
        vs()[v].edges.push_back(res);
        return res;
    }

    int opp(int u, int e) {
        if (es()[e].u == u) return es()[e].v;
        return es()[e].u;
    }

    int& eNext(int u, int e) {
        if (es()[e].u == u) return es()[e].uNext;
        return es()[e].vNext;
    }

    void enumerate() {
        for (int i=0; i<(int)vs().size(); ++i) {
            vs()[i].ind = i;
        }
        for (int i=0; i<(int)es().size(); ++i) {
            es()[i].ind = i;
        }
    }
};

void embed(PlanarGraph& g);
void triangulate(PlanarGraph& g);

#endif
