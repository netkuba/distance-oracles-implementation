#ifndef _BOOST_WRAPPER_H_
#define _BOOST_WRAPPER_H_

#include "graph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/is_kuratowski_subgraph.hpp>

using namespace boost;

template <class VertexValue, class EdgeValue>
Graph<VertexValue, EdgeValue>
constructPlanarGraph(
        vector<VertexValue> vVal,
        vector<EdgeValue> eVal,
        vector< pair<int, int> > e) {
    typedef class Graph<VertexValue, EdgeValue>
        Graph;
    typedef class Graph::Edge
        Edge;
    typedef class Graph::Vertex
        Vertex;

    struct ptr_t {
        typedef edge_property_tag kind;
    };

    typedef adjacency_list< vecS, vecS, undirectedS, property<vertex_index_t, int>, property<ptr_t, Edge*> > 
        graph_t;
//    typedef class graph_traits<graph_t>::vertex_descriptor
//        vertex_descriptor_t;
    typedef class graph_traits<graph_t>::edge_descriptor
        edge_descriptor_t;
    typedef class graph_traits<graph_t>::vertex_iterator 
        vertex_iterator_t;
    typedef class std::vector< std::vector< class graph_traits<graph_t>::edge_descriptor > > 
        embedding_storage_t;
    typedef boost::iterator_property_map< class embedding_storage_t::iterator, class property_map<graph_t, vertex_index_t>::type >
        embedding_t;

        
    graph_t g(vVal.size());
    Graph result;

    typedef unsigned int uint;

    for (uint i=0; i<vVal.size(); ++i) {
        result.vs().push_back(Vertex(vVal[i]));
    }
    result.es().reserve(eVal.size());
    for (uint i=0; i<eVal.size(); ++i) {
        result.es().push_back(Edge(
            eVal[i], 
            &result.vs()[e[i].first],
            &result.vs()[e[i].second]
        ));
        add_edge(e[i].first, e[i].second, &result.es().back(), g);
    }

    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
            boyer_myrvold_params::embedding = embedding
            );

    vertex_iterator_t vi, vi_end;
    tie(vi, vi_end) = vertices(g);
    for (; vi != vi_end; ++vi) {
        int v = get(vertex_index, g)[*vi];

        for (auto it = embedding[*vi].begin(); it != embedding[*vi].end(); ++it) {
           result.vs()[v].edges.push_back(get(ptr_t(), g)[*it]);
        }

    }

    result.updateIndices();
    return result;
}

#endif
