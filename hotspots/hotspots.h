#ifndef _HOTSPOTS_H_
#define _HOTSPOTS_H_

#include <atomic>
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/hash_table.h"

template <typename vertex, typename graph>
auto hotspots(parlay::sequence<vertex> U, const graph& G) {
	vertex n = G.size();

	// n indicates that the vertex has not been visited yet
	auto nearest = parlay::tabulate<std::atomic<vertex>>(n, [&] (long i) {return n;});

	// mark the set U as visited from self
	parlay::parallel_for(0, U.size(), [&] (long i) {
			nearest[U[i]] = U[i];
    });
	// the general algorithm for this
	// 	frontier: set = U
	// 	next_frontier: set = {}
	// while frontier.not_empty
	//		next_frontier = {}
	// 		for v in frontier
	//
	// 			for n in v.neighbours
	// 				nearest[n] = nearest[v];
	// 				next_frontier.add(n);
	// 		frontier = next_frontier
	//		next_frontier = {}

	// for the purpose of this project, I'll use parlay::hash_map in lieu of a set

	// initialize sets
	// graph is defined as a sequence of sequence of ints
	// so in the frontiers we'll store the indices of the vertexes
	parlay::hashtable<parlay::hash_numeric<vertex>>* frontier = new parlay::hashtable(G.size(), parlay::hash_numeric<vertex>{});
	std::cout << "frontier length: " <<  frontier->count() << std::endl;
	parlay::hashtable<parlay::hash_numeric<vertex>>*  next_frontier;

	// add U as first frontier
//	parlay::parallel_for(0, U.size(), [&] (long i) {
//		frontier->insert(U[i]);
//	});
	for(long i = 0; i < U.size(); i++) {
		frontier->insert(U[i]);
	}

//	while (frontier->count() > 0) {
//		next_frontier = new parlay::hashtable(G.size(), parlay::hash_numeric<vertex>{});
//
////		auto frontier_indices = frontier->get_index();
//		auto frontier_entries = frontier->entries();
//
////		parlay::parallel_for(0, frontier_entries.size(), [&] (long i) {
////			// get sequence of vertices representing the neighbour of the current vertex
////			parlay::sequence<vertex> neighbours = G[frontier_entries[i]];
////			// get hotspot
////			vertex nearest_hotspot = nearest[frontier_entries[i]];
////			parlay::parallel_for(0, neighbours.size(), [&] (long j) {
////				vertex neighbour = neighbours[j];
////				nearest[neighbour] = nearest_hotspot;
////				next_frontier->insert(neighbour);
////			});
////		});
// 		for(long i = 0; i < frontier_entries->size(); i++) {
//			// get sequence of vertices representing the neighbour of the current vertex
//			parlay::sequence<vertex> neighbours = G[frontier_entries[i]];
//			// get hotspot
//			vertex nearest_hotspot = nearest[frontier_entries[i]];
//			for(long j = 0; j < neighbours.size(); j++) {
//				vertex neighbour = neighbours[j];
//				nearest[neighbour] = nearest_hotspot;
//				next_frontier->insert(neighbour);
//			}
//		}
//
//		// swap next frontier and frontier
//		auto tmp = next_frontier;
//		next_frontier = frontier;
//		frontier = tmp;
//
//		// reset next frontier -- currently doing this by deleting and reallocating at the beggining
//		delete next_frontier;
//	}

	// cleanup -- delete frontier
	delete frontier;


	// to be filled in
	// std::cout << "unimplemented" << std::endl;



	// convert from sequence of atomics to a regular sequence
	return parlay::tabulate(n, [&] (long i) {return nearest[i].load();});
}

#endif
