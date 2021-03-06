/*
 * Vertex.h
 *
 *  Created on: Nov 25, 2014
 *      Author: sohamgrad
 */

#ifndef LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_VERTEX_H_
#define LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_VERTEX_H_

#include <vector>
#include <string>
#include "Edge.h"

class Edge;
/////////////////    Vertex Class   //////////////////////
class Vertex {
private:
	std::vector<Edge*> * outEdges;
	std::vector<Edge *> * inEdges;
	std::vector<std::string> * variables;

public:

	// make a vertex with no outgoing edges
	Vertex(std::string initialVariable);

	// make a vertex with an outgoing edge to a target vertex
	Vertex(std::string initialVariable, Vertex* initialTarget);

	~Vertex();

	void removeInEdge(Edge * edge);

	void removeOutEdge(Edge * edge);

	// add an edge to a pre-constructed target Vertex
	void addTarget(Vertex* targetVertex);


	// add an edge to a newly constructed vertex
	void addTarget(std::string targetVar);

	// check if a vertex has an edge to the input target vertex
	// (used so don't add duplicates in addTargetsOfOther
	bool alreadyHasEdge(Vertex * targetVertex);

	// given another vertex, this method copies every edge outgoing from the other vertex
	// and adds to this one
	void addTargetsOfOther(Vertex * otherVertex);

	std::string getFirstLabel();

	std::vector<Edge *> * getOutEdges();

	std::vector<Edge *> * getInEdges();

	//Get all the vertices pointed by this vertex i.e the vertices the outedges point to
	std::vector<Vertex *> *getOutVertices();

	std::vector<std::string> * getLabels();

	void takeLabels(Vertex * other);

	std::string toString();
};

#endif /* LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_VERTEX_H_ */
