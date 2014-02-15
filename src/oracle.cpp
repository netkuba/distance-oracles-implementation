#include "oracle.h"

#include <algorithm>
#include <utility>
using std::min;
using std::max;
using std::make_pair;
using std::sort;

PlanarOracle::PlanarOracle(
        int n,
        vector< pair< int, int > > edge, 
        vector< W > weight,
        W eps) : graph(n), vertex_to_portal(n) {

    for (int i=0; i<(int)edge.size(); ++i) {
        graph.add_edge(edge[i].first, edge[i].second, weight[i]);
    }
    
    double minD = infinity, maxD = 0;
    {
        vector<W> distance;
        getDistances(graph, 0, distance);
        for (int i=1; i<(int)distance.size(); ++i) {
            minD = min(minD, distance[i]);
            maxD = max(maxD, distance[i]);
        }
        maxD *= 2;
    }

    vector< W > preAlpha;
    vector< vector<int> > prePortals;

    {
        vector< vector<int> > parents;
        W alpha = minD;
        while (alpha <= maxD) {
            vector< vector<int> > tmpParents, tmpMappings;
            vector< PlanarGraph > tmpSubgs;
            getAlphaFamily(graph, alpha, tmpSubgs, tmpMappings, tmpParents);

            for (int i=0; i<(int)tmpSubgs.size(); ++i) {

                for (int j=1; j<(int)tmpMappings[i].size(); ++j) {
                    assert(tmpMappings[i][j] != -1);
                }

                pieces.push_back(make_pair(tmpSubgs[i], tmpMappings[i]));
                parents.push_back(tmpParents[i]);
                preAlpha.push_back(alpha);
            }

            alpha *= 2;
        }

        for (int i=0; i<(int)pieces.size(); ++i) {

            prePortals.push_back(vector<int>());
            PlanarGraph pg = pieces[i].first;
            vector<int> mapping = pieces[i].second;
            alpha = preAlpha[i];

            if (pg.vs().size() <= 3) {
                for (int v=0; v<(int)pg.vs().size(); ++v) {
                    prePortals.back().push_back(v);
                }
                continue;
            }

            vector< vector<int> > tmpParents, tmpMappings;
            vector< PlanarGraph > tmpSubgs;
            vector< vector< pair<int, int> > > tmpPaths;
            
            subdivide(pg, parents[i], tmpSubgs, tmpMappings, 
                    tmpParents, tmpPaths);
/*
            printf("Robie %d (%d)\n", i, pg.es().size());
            for (int j=0; j<pg.es().size(); ++j) {
                printf("%lf ", pg.es()[j].w);
            }
            printf("\n");

            for (int j=0; j<(int)tmpPaths.size(); ++j) {
                auto v = tmpPaths[j];
                for (auto u: v) {
                    printf("%d (%lf) ", u.first, pg.es()[u.second].w);
                }
                printf("\n");
            }
            printf("\n");
*/
            for (int j=0; j<(int)tmpSubgs.size(); ++j) {
                for (int k=0; k<(int)tmpMappings[j].size(); ++k) {
                    if (tmpMappings[j][k] == -1) continue;
                    tmpMappings[j][k] = mapping[tmpMappings[j][k]];
                }
                pieces.push_back(make_pair(tmpSubgs[j], tmpMappings[j]));
                parents.push_back(tmpParents[j]);
                preAlpha.push_back(alpha);
            }
            for (int j=0; j<(int)tmpPaths.size(); ++j) {
                pair<int, int> prevV(-1, -1);
                W dist = 0;
                for (auto v: tmpPaths[j]) {
                    if (dist > alpha*eps/4) {
                        prePortals.back().push_back(prevV.first);
                        dist = pg.es()[prevV.second].w;
                    }
                    dist += pg.es()[v.second].w;
                    prevV = v;
                }
            }
/*
            printf("Dostaje\n");
            for (auto v: prePortals.back()) {
                printf("%d ", v);
            }
            printf("\n");
*/
            sort(prePortals.back().begin(), prePortals.back().end());
            auto it = unique(prePortals.back().begin(), prePortals.back().end());
            prePortals.back().resize(std::distance(prePortals.back().begin(), it));
        }
    }

    {
        for (int i=0; i<(int)prePortals.size(); ++i) {
            vector<W> distances;
            for (int p: prePortals[i]) {
                getDistances(pieces[i].first, p, distances);
                portals.push_back(Portal(i, p));
                vector<int> mapping = pieces[i].second;
                for (int j=0; j<(int)mapping.size(); ++j) {
                    if (mapping[j] == -1) continue;
                    int v = mapping[j];
                    portals.back().to_vertex[v] = distances[j];
                    vertex_to_portal[v][portals.size()-1] = distances[j];
                }
            }
        }
    }
}



W PlanarOracle::distance_to_vertex(int v, int u) {
    W res = infinity;
    for (auto vp: vertex_to_portal[v]) {
        int pp = portals[vp.first].p;
        int vv = portals[vp.first].v;
        auto pu = portals[vp.first].to_vertex.find(u);
        if (pu == portals[vp.first].to_vertex.end()) continue;
        res = min(res, vp.second + pu->second);
    }
    return res;
}

W PlanarOracle::exact_to_vertex(int v, int u) {
    vector<W> distances;
    getDistances(graph, v, distances);
    return distances[u];
}
