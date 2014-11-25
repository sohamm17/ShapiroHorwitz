// this file is used to test the GraphClasses methods

#include "GraphClasses.h"


int testVertex()
{
	// just to see if the methods run and don't crash or anything
	// won't check for correctness
	Vertex * v1 = new Vertex("v1");
	Vertex * v2 = new Vertex("v2", v1, Edge::MAY);
	v1->addTarget(v2, Edge::MAY);
	v1->addTarget("v3", Edge::MUST);
	Vertex * v4 = new Vertex("v4");
	v4->addTargetsOfOther(v1);
	return 0;

}

int testGraph()
{
	Graph * testG = new Graph();

	// vertex no edges
	testG->createVertex(std::string("A1"));

	// attach new vertex A2 to A1
	testG->createVertex(std::string("A2"), std::string("A1"), Edge::MAY);

	// clone A2
	testG->cloneVertex(std::string("A3"), std::string("A2"));



	testG->createVertices("A4", "A5", Edge::MAY);

	testG->createVertex("A6");
	testG->createEdge("A4", "A6", Edge::MAY);



	// given two labels correspondng to vertices A and B, this method adds edges from all children of A
	// to all children of B. Called to process "store" calls in LLVM
	testG->storeConnect("A2", "A4");

	testG->createVertex("A7");
	testG->createVertex("A8", "A7", Edge::MAY);
	testG->createEdge("A2", "A8", Edge::MAY);

	testG->loadConnect("A2", "A9");

	testG->createDotFile("A.dot");
	return 0;
}
/*
class Graph

	// given two labels correspondng to existing vertex A and to-be-constructed vetex B,
	// this method creates B with outgoing edges copied from all children of A
	// Called to process "load" calls in LLVM
	void loadConnect(std::string a, std::string b);


	// getter method
	std::vector<Vertex*> * getVertices() { return vertices;}


private:
	// given a label, returns the vertex which the label corresponds to
	Vertex * getVertexAtLabel(std::string label);

	// given two vectors of edge pointers E1 and E2, this method adds edges from the target of edges in E1
	// to the target of all edges in E2
	void fullConnectSets(std::vector<Edge*> * E1, std::vector<Edge*> * E2);
};


*/

int main()
{
	testVertex();
	testGraph();


	return 0;
}
