/*
 * Edge.h
 *
 *  Created on: Nov 25, 2014
 *      Author: sohamgrad
 */

#ifndef LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_EDGE_H_
#define LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_EDGE_H_

#include <string>
#include "Vertex.h"

class Vertex;
// no need to manually create edges. Use vertex methods for that
class Edge {
friend class Vertex;
public:
//	enum EdgeType {
//		MAY = 0,
//		MUST = 1
//	};
//
//	EdgeType getType();

	Vertex * getTarget();

private:
	Vertex * target;
	Vertex * source;
//	EdgeType type;

	Edge(Vertex * target);

};


#endif /* LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_EDGE_H_ */
