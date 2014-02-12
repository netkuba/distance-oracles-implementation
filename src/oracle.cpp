#include "oracle.h"

#include <utility>
using std::min;
using std::max;
using std::make_pair;

PlanarOracle::PlanarOracle(
        int n,
        vector< pair< int, int > > edge, 
        vector< W > weight,
        W eps) : graph(n), clusters(n) {

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

    {
        vector< vector<int> > parents;
        W alpha = minD;
        while (alpha <= maxD) {
            vector< vector<int> > tmpParents, tmpMappings;
            vector< PlanarGraph > tmpSubgs;
            getAlphaFamily(graph, alpha, tmpSubgs, tmpMappings, tmpParents);

            for (int i=0; i<(int)tmpSubgs.size(); ++i) {
                pieces.push_back(make_pair(tmpSubgs[i], tmpMappings[i]));
                parents.push_back(tmpParents[i]);
            }

            alpha *= 2;
        }

        for (int i=0; i<(int)pieces.size(); ++i) {
            portals.push_back(vector<int>());
            PlanarGraph& pg = pieces[i].first;
            vector<int>& mapping = pieces[i].second;

            if (pg.vs().size() <= 3) {
                for (int v=0; v<(int)pg.vs().size(); ++v) {
                    portals.back().push_back(v);
                }
                for (int v=0; v<(int)portals.back().size(); ++v) {
                    for (int u=0; u<(int)mapping.size(); ++u) {
                        if (mapping[u] == -1) continue;
                        clusters[mapping[u]].push_back(
                                make_pair(i, v));
                    }
                }
                continue;
            }

            vector< vector<int> > tmpParents, tmpMappings;
            vector< PlanarGraph > tmpSubgs;
            vector< vector< pair<int, int> > > tmpPaths;
            
            subdivide(pg, parents[i], tmpSubgs, tmpMappings, 
                    tmpParents, tmpPaths);

            for (int j=0; j<(int)tmpSubgs.size(); ++j) {
                for (int k=0; k<(int)tmpMappings[j].size(); ++k) {
                    if (tmpMappings[j][k] == -1) continue;
                    tmpMappings[j][k] = mapping[tmpMappings[j][k]];
                }
                pieces.push_back(make_pair(tmpSubgs[j], tmpMappings[j]));
                parents.push_back(tmpParents[j]);
            }

            for (int j=0; j<(int)tmpPaths.size(); ++j) {
                pair<int, int> prevV(-1, -1);
                W dist = 0; 
                for (auto v: tmpPaths[j]) {
                    if (dist > alpha*eps/4) {
                        portals.back().push_back(prevV.first);
                        dist = prevV.second;
                    }
                    dist += v.second;
                    prevV = v;
                }
            }
            for (int v=0; v<(int)portals.back().size(); ++v) {
                for (int u=0; u<(int)mapping.size(); ++u) {
                    if (mapping[u] == -1) continue;
                    clusters[mapping[u]].push_back(
                            make_pair(i, v));
                }
            }
        }
    }
}
