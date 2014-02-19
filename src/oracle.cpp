#include "oracle.h"

#include <algorithm>
#include <utility>
using std::min;
using std::max;
using std::make_pair;
using std::sort;
using std::swap;

void PlanarOracle::processLeaf(
        int i,
        PlanarGraph pg,
        vector<int>& mapping) {
    for (int v=0; v<(int)pg.vs().size(); ++v) {
        if (mapping[v] == -1) continue;
        vertices[mapping[v]].direct_pieces.push_back(i);
    }
    /*
    vector<int> newPortals;
    for (int v=0; v<(int)pg.vs().size(); ++v) {
        newPortals.push_back(v);
    }
    processPortals(i, pg, mapping, newPortals);
    */
}

void PlanarOracle::processPortals(
        int i,
        PlanarGraph& pg,
        vector<int>& mapping,
        vector<int>& newPortals) {
    
    vector<W> distances;
    for (int p: newPortals) {
        getDistances(pg, p, distances);
        portals.push_back(Portal(i, p));
        for (int j=0; j<(int)mapping.size(); ++j) {
            if (mapping[j] == -1) continue;
            int v = mapping[j];

            portals.back().N[v] = distances[j];
            vertices[v].to_portal[portals.size()-1] = distances[j];
            labels[v].L.push_back(make_pair(v, i));
        }
    }
}

PlanarOracle::PlanarOracle(
        int n,
        vector< pair< int, int > > edge, 
        vector< W > weight,
        W eps) : graph(n), vertices(n), labels(n), fu(n) {

    for (int i=0; i<(int)edge.size(); ++i) {
        graph.add_edge(edge[i].first, edge[i].second, weight[i]);
    }
    embed(graph);

    
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


            PlanarGraph pg = pieces[i].first;
            vector<int> mapping = pieces[i].second;
            alpha = preAlpha[i];

            if (pg.vs().size() <= ro) {
                processLeaf(i, pg, mapping);
                continue;
            }

            vector< vector<int> > tmpParents, tmpMappings;
            vector< PlanarGraph > tmpSubgs;
            vector< vector< pair<int, int> > > tmpPaths;
            vector<int> newPortals;
            
            subdivide(pg, parents[i], tmpSubgs, tmpMappings, 
                    tmpParents, tmpPaths);
            
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
                        newPortals.push_back(prevV.first);
                        dist = pg.es()[prevV.second].w;
                    }
                    dist += pg.es()[v.second].w;
                    prevV = v;
                }
            }

            sort(newPortals.begin(), newPortals.end());
            auto it = unique(newPortals.begin(), newPortals.end());
            newPortals.resize(std::distance(newPortals.begin(), it));

            processPortals(i, pg, mapping, newPortals);
        }
    }
}

void PlanarOracle::activate(int v) {
    labels[v].active = true;
    for (auto l: labels[v].L) {
        int p = l.second;
        portals[p].H.insert(make_pair(portals[p].N[v], v));
    }
}

int PlanarOracle::merge(int v, int u) {
    if (labels[v].L.size() < labels[u].L.size()) swap(u,v);
    if (labels[v].active) {
        for (auto l: labels[u].L) {
            int p = l.second;

            W du = portals[p].N[u];
            portals[p].N.erase(u);
            portals[p].H.erase(make_pair(du, u));

            auto it = portals[p].N.find(v);
            if (it != portals[p].N.end()) {
                W dv = it->second;
                W du = min(du, dv);
                portals[p].H.erase(make_pair(dv, v));
            }

            portals[p].N[v] = du;
            portals[p].H.insert(make_pair(du, v));
        }
    }

    labels[v].L.insert(labels[v].L.end(), labels[u].L.begin(), labels[u].L.end());
    labels[u].L.clear();

    fu.unionn(v, u);
    return v;
}

W PlanarOracle::distance_to_color(int v, int u) {
    W res = infinity;
    for (auto it: vertices[v].to_portal) {
        int p = it.first;
        auto itt = portals[p].N.find(u);
        if (itt == portals[p].N.end()) continue;
        res = min(res, it.second + itt->second);
    }
    for (auto it: vertices[v].direct_pieces) {
        vector<int> mapping = pieces[it].second;
        int s = -1;
        for (int i=0; i<(int)mapping.size(); ++i) if (mapping[i] == v) s = i;

        vector<W> distances;
        getDistances(pieces[it].first, s, distances);
        for (int i=0; i<(int)mapping.size(); ++i) {
            if (isOfColor(mapping[i], u)) {
                res = min(res, distances[i]);
            }
        }
    }
    return res;
}

W PlanarOracle::distance_to_closest(int v) {
    W res = infinity;
    for (auto it: vertices[v].to_portal) {
        int p = it.first;
        if (portals[p].H.empty()) continue;
        res = min(res, it.second + portals[p].H.begin()->first);
    }
    return res;
}

W PlanarOracle::exact_to_vertex(int v, int u) {
    vector<W> distances;
    getDistances(graph, v, distances);
    return distances[u];
}
