#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/is_kuratowski_subgraph.hpp>

using namespace boost;
using std::cin;
using std::cout;
using std::endl;

typedef adjacency_list< vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int> > 
    graph_t;
typedef graph_traits<graph_t>::vertex_descriptor
    vertex_descriptor_t;
typedef graph_traits<graph_t>::edge_descriptor
    edge_descriptor_t;
typedef graph_traits<graph_t>::vertex_iterator 
    vertex_iterator_t;
typedef std::vector< std::vector< graph_traits<graph_t>::edge_descriptor > > 
    embedding_storage_t;
typedef boost::iterator_property_map< embedding_storage_t::iterator, property_map<graph_t, vertex_index_t>::type >
    embedding_t;


int main() {

    graph_t g(7);
    add_edge(0,1,g);
    add_edge(1,2,g);
    add_edge(2,3,g);
    add_edge(3,0,g);
    add_edge(3,4,g);
    add_edge(4,5,g);
    add_edge(5,6,g);
    add_edge(6,3,g);
    add_edge(0,4,g);
    add_edge(1,3,g);
    add_edge(3,5,g);
    add_edge(2,6,g);
    add_edge(1,4,g);
    add_edge(1,5,g);
    add_edge(1,6,g);


    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index,g));

    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
            boyer_myrvold_params::embedding = embedding
            );

    vertex_iterator_t vi, vi_end;
    tie(vi, vi_end) = vertices(g);

    for (; vi != vi_end; ++vi) {
        cout << *vi << ":" << endl;
        for (auto it = embedding[*vi].begin(); it != embedding[*vi].end(); ++it) {
            cout << *it << endl;
        }
    }
    
}
