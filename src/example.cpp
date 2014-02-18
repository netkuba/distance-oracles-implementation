#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/make_biconnected_planar.hpp>

using namespace boost;

#include <utility>
#include <vector>
#include <cstdio>

using std::pair;
using std::vector;
using std::printf;

typedef adjacency_list< vecS, vecS, undirectedS, property<vertex_index_t, int>, property<edge_index_t, int> > 
    graph_t;
typedef graph_traits<graph_t>::edge_descriptor
    edge_descriptor_t;
typedef graph_traits<graph_t>::edge_iterator
    edge_iterator_t;
typedef vector< vector< edge_descriptor_t > >
    embedding_storage_t;
typedef boost::iterator_property_map< embedding_storage_t::iterator, property_map<graph_t, vertex_index_t>::type >
    embedding_t;


int main() {
    vector< pair< edge_descriptor_t, bool > > eds;

    graph_t g(11);
    eds.push_back(add_edge(0,1,0,g));
    eds.push_back(add_edge(1,3,1,g));
    eds.push_back(add_edge(1,2,2,g));
    eds.push_back(add_edge(4,1,3,g));
    eds.push_back(add_edge(2,4,4,g));
    eds.push_back(add_edge(2,3,5,g));
    eds.push_back(add_edge(3,4,6,g));
    eds.push_back(add_edge(5,6,7,g));
    eds.push_back(add_edge(7,5,8,g));
    eds.push_back(add_edge(5,8,9,g));
    eds.push_back(add_edge(6,7,10,g));
    eds.push_back(add_edge(8,6,11,g));
    eds.push_back(add_edge(6,9,12,g));
    eds.push_back(add_edge(7,8,13,g));
    eds.push_back(add_edge(9,7,14,g));
    eds.push_back(add_edge(0,8,15,g));
    eds.push_back(add_edge(8,9,16,g));
    eds.push_back(add_edge(9,10,17,g));

    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    for (int i=0; i<(int)eds.size(); ++i) {
        if (!eds[i].second) {
            printf("Incorrect input\n");
            return 0;
        }
    }

    embedding[0].push_back(eds[0].first);
    embedding[0].push_back(eds[15].first);
    embedding[1].push_back(eds[0].first);
    embedding[1].push_back(eds[2].first);
    embedding[1].push_back(eds[3].first);
    embedding[1].push_back(eds[1].first);
    embedding[2].push_back(eds[2].first);
    embedding[2].push_back(eds[5].first);
    embedding[2].push_back(eds[4].first);
    embedding[3].push_back(eds[1].first);
    embedding[3].push_back(eds[6].first);
    embedding[3].push_back(eds[5].first);
    embedding[4].push_back(eds[3].first);
    embedding[4].push_back(eds[4].first);
    embedding[4].push_back(eds[6].first);
    embedding[5].push_back(eds[7].first);
    embedding[5].push_back(eds[9].first);
    embedding[5].push_back(eds[8].first);
    embedding[6].push_back(eds[7].first);
    embedding[6].push_back(eds[10].first);
    embedding[6].push_back(eds[12].first);
    embedding[6].push_back(eds[11].first);
    embedding[7].push_back(eds[8].first);
    embedding[7].push_back(eds[13].first);
    embedding[7].push_back(eds[14].first);
    embedding[7].push_back(eds[10].first);
    embedding[8].push_back(eds[9].first);
    embedding[8].push_back(eds[11].first);
    embedding[8].push_back(eds[16].first);
    embedding[8].push_back(eds[15].first);
    embedding[8].push_back(eds[13].first);
    embedding[9].push_back(eds[12].first);
    embedding[9].push_back(eds[14].first);
    embedding[9].push_back(eds[17].first);
    embedding[9].push_back(eds[16].first);
    embedding[10].push_back(eds[17].first);
/*
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                boyer_myrvold_params::embedding = embedding)) {
        printf("OK!\n");
    } else {
        printf("ERROR!\n");
    }
*/    
    make_biconnected_planar(g, embedding);
    {
        edge_iterator_t ei, ei_end;
        int edge_count = 0;
        for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
            put(get(edge_index, g), *ei, edge_count++);
        }
    }

    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                boyer_myrvold_params::embedding = embedding)) {
        printf("OK!\n");
    } else {
        printf("ERROR!\n");
    }

    return 0;
}
