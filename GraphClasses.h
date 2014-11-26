#include <vector>
#include <string>
#include <map>
#include "Edge.h"
#include "Vertex.h"



class Vertex;

/////////////////    Graph Class    ////////////////////////
class Graph {
private:
	std::vector<Vertex*> * vertices;
	std::map<std::string, Vertex*> * vertexMap;
	std::map<std::string, int> * categoryMap;

public:
	// to create a new vertex which is the source. Finds the vertex corresponding
	// to the input target label and attaches the new source vertex to it.
	// Vertex is added to the graph.
	void createVertex(std::string sourceVar, std::string targetVar);

	// to create a new vertex without edges and add it to the graph.
	Vertex * createVertex(std::string vertexLabel);


	// given two variables and the edge type (MAY or MUST), this method will
	// construct two new vertices, construct an edge between them, and add the
	// vertices to the graph
	void createVertices(std::string sourceVar, std::string targetVar);

	// given labels corresponding to vertices, this method adds an edge from the source
	// to the target
	bool createEdge(std::string sourceVar, std::string targetVar);


	// given the label of a current vertex and a new label, this method constructs a new
	// vertex with the same outgoing edges as the old vertex. Adds to graph
	// if clong is successful, return true, otherwise false
	bool cloneVertex(std::string newLabel, std::string oldLabel);


	// given two labels correspondng to vertices A and B, this method adds edges from all children of A
	// to all children of B. Called to process "store" calls in LLVM
	void storeConnect(std::string a, std::string b);

	// given two labels correspondng to existing vertex A and to-be-constructed vetex B,
	// this method creates B with outgoing edges copied from all children of A
	// Called to process "load" calls in LLVM
	void loadConnect(std::string a, std::string b);

	// creates a dot file of the graph for visual inspection
	void createDotFile(std::string fileName);

	void unionizeVertices(std::string a, std::string b);

	//default constructor. Makes an empty graph
	Graph(std::map<std::string, int> * categoryMap);

	// getter method
	std::vector<Vertex*> * getVertices() { return vertices;}


private:
	// given a label, returns the vertex which the label corresponds to
	Vertex * getVertexAtLabel(std::string label);

	// given two vectors of edge pointers E1 and E2, this method adds edges from the target of edges in E1
	// to the target of all edges in E2
	void fullConnectSets(std::vector<Edge*> * E1, std::vector<Edge*> * E2);
};









