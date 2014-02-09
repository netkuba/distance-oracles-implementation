#include "graph.h"
#include "boost_wrapper.h"
#include <cstdio>
#include <utility>

using namespace std;

int main() {
    vector<SuperVVal> vVal;
    vector<SuperEVal> eVal;
    vector< pair< int, int > > edges;

    int n = 5;
    for (int i=0; i<n*n; ++i) vVal.push_back(1);
    for (int i=0; i<n*(n-1)*2; ++i) eVal.push_back(1);
    for (int i=0; i<n-1; ++i) for (int j=0; j<n; ++j) edges.push_back(make_pair(i*n+j, (i+1)*n+j));
    for (int i=0; i<n; ++i) for (int j=0; j<n-1; ++j) edges.push_back(make_pair(i*n+j, i*n+j+1));
/*
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
*/
    G g = constructPlanarGraph<SuperVVal, SuperEVal>(vVal, eVal, edges); 

    for (int i=0; i<g.vs().size(); ++i) {
        printf("Vertex %d: (%d)\n", i, g.vs()[i].edges.size());
        for (auto edge: g.vs()[i].edges) {
            printf("%d - %d : %.1Lf\n", edge->u->value, edge->v->value, edge->value.w);
        }
    }

    printf("%d %d!\n", g.vs().size(), g.es().size());
    pair<long double, long double> stretch = getStretch(g);

    printf("%Lf - %Lf\n", stretch.first, stretch.second);

    vector<SubG> result = getAlphaFamily(g, 1); 
    for (auto subg: result) {
        printf("SUBGRAPH\n");
        for (auto v: subg.vs()) {
            printf("Vertex: %d (%d)\n", v.ind, v.value.super != NULL ? v.value.super->ind : -1);
            for (auto e: v.edges) {
                printf("%d - %d (%d - %d) \n", e->u->ind, e->v->ind, 
                        e->u->value.super != NULL ? e->u->value.super->ind : -1,
                        e->v->value.super != NULL ? e->v->value.super->ind : -1);
            }
        }
        printf("\n");
/*
        for (auto e: subg.es()) {
            printf("%d - %d (%d - %d) \n", e.u->ind, e.v->ind, 
                    e.u->value.super != NULL ? e.u->value.super->ind : -1,
                    e.v->value.super != NULL ? e.v->value.super->ind : -1);
        }
*/
    }

    return 0;
}
