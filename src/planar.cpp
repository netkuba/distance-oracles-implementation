#include "planar.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/is_kuratowski_subgraph.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>

using namespace boost;
    
typedef adjacency_list< vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int> > 
    graph_t;
typedef graph_traits<graph_t>::edge_descriptor
    edge_descriptor_t;
typedef graph_traits<graph_t>::vertex_iterator
    vertex_iterator_t;
typedef vector< vector< edge_descriptor_t > >
    embedding_storage_t;
typedef boost::iterator_property_map< embedding_storage_t::iterator, property_map<graph_t, vertex_index_t>::type >
    embedding_t;


void emToPg(graph_t& g, embedding_t& embedding, PlanarGraph& pg) {
    vertex_iterator_t vi, vi_end;
    tie(vi, vi_end) = vertices(g);
    for (; vi != vi_end; ++vi) {
        int u = get(vertex_index, g)[*vi];
        auto it_next = embedding[*vi].begin(), it = it_next;
        if (it_next == embedding[*vi].end()) continue;
        ++it_next;
        for (; it_next != embedding[*vi].end(); ++it_next, ++it) {
            int e = get(edge_index, g)[*it];
            int f = get(edge_index, g)[*it_next];
            pg.eNext(u, e) = f;
        }
        int e = get(edge_index, g)[*it];
        pg.eNext(u, e) = get(edge_index, g)[*embedding[*vi].begin()];
    }
    
    return;
}

void pgToEm(PlanarGraph& pg, graph_t& g, embedding_t& embedding) {
    vector<graph_t::edge_descriptor> eds;
    for (int i=0; i<(int)pg.es().size(); ++i) {
        eds.push_back(add_edge(pg.es()[i].u, pg.es()[i].v, i, g).first); 
    }

    for (int v=0; v<(int)pg.vs().size(); ++v) {
        embedding[v].clear();
        if (pg.vs()[v].edges.empty()) continue;
        int e_begin = pg.vs()[v].edges[0], e = e_begin;
        do {
            embedding[v].push_back(eds[e]);
            e = pg.eNext(v, e);
        } while (e != e_begin);    
    }
    return;
}

void fillEdges(PlanarGraph& pg, graph_t& g) {
    graph_traits<graph_t>::edge_iterator ei, ei_end;
    int eCounter = 0;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
        if (eCounter >= (int)pg.es().size()) {
            pg.add_edge(source(*ei, g), target(*ei, g));
        }
        put(get(edge_index, g), *ei, eCounter);
        ++eCounter;
    }
}

void embed(PlanarGraph& pg) {
    graph_t g(pg.vs().size());
    for (int i=0; i<(int)pg.es().size(); ++i) {
        add_edge(pg.es()[i].u, pg.es()[i].v, i, g); 
    }

    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
            boyer_myrvold_params::embedding = embedding
            );
    
    emToPg(g, embedding, pg);
    return;
}

void triangulate(PlanarGraph& pg) {
    graph_t g(pg.vs().size());
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    pgToEm(pg, g, embedding);
    
    make_biconnected_planar(g, embedding);
    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
            boyer_myrvold_params::embedding = embedding
            );
    fillEdges(pg, g);

    make_maximal_planar(g, embedding);
    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
            boyer_myrvold_params::embedding = embedding
            );
    fillEdges(pg, g);

    emToPg(g, embedding, pg);
    return;
}