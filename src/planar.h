#ifndef _PLANAR_H_
#define _PLANAR_H_

#include <vector>
#include <utility>

using std::vector;

class PlanarGraph {
public:    
    struct Edge {
        Edge(int uu, int vv) : 
            ind(-1), u(uu), v(vv) {}

        int ind;
        int u, v;
        int uNext, vNext;
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

    int add_edge(int u, int v) {
        int res = es().size();
        es().push_back(Edge(u, v));
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
