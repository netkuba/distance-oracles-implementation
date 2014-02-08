#include "graph.h"
#include <cstdio>
#include <utility>

using namespace std;

typedef int EdgeValue;
typedef int VertexValue;
typedef Graph<VertexValue, EdgeValue> G;

int main() {
    vector<int> vVal, eVal;
    vector< pair< int, int > > edges;
    for (int i=0; i<7; ++i) vVal.push_back(i);
    for (int i=0; i<15; ++i) eVal.push_back(i);
    edges.push_back(make_pair(0,1));
    edges.push_back(make_pair(1,2));
    edges.push_back(make_pair(2,3));
    edges.push_back(make_pair(3,0));
    edges.push_back(make_pair(3,4));
    edges.push_back(make_pair(4,5));
    edges.push_back(make_pair(5,6));
    edges.push_back(make_pair(6,3));
    edges.push_back(make_pair(0,4));
    edges.push_back(make_pair(1,3));
    edges.push_back(make_pair(3,5));
    edges.push_back(make_pair(2,6));
    edges.push_back(make_pair(1,4));
    edges.push_back(make_pair(1,5));
    edges.push_back(make_pair(1,6));

    G g = constructPlanarGraph(vVal, eVal, edges); 

    for (int i=0; i<g.vertices.size(); ++i) {
        printf("Vertex %d: (%d)\n", i, g.vertices[i].edges.size());
        for (auto edge: g.vertices[i].edges) {
            printf("%d - %d\n", edge->u->value, edge->v->value);
        }
    }

    return 0;
}
