#include <vector>
#include <string>



class Vertex;


/////////////////    Edge class    ///////////////////////


// no need to manually create edges. Use vertex methods for that
class Edge {
friend class Vertex;
public:
	enum EdgeType {
		MAY = 0,
		MUST = 1
	};

	Vertex * getTarget();

private:
	Vertex * target;
	EdgeType type;

	Edge(Vertex * target, EdgeType type);

};


/////////////////    Vertex Class   //////////////////////
class Vertex {
private:
	std::vector<Edge*> * outEdges;
	std::vector<std::string> * variables;

public:


	// make a vertex with no outgoing edges
	Vertex(std::string initialVariable);

	// make a vertex with an outgoing edge to a target vertex
	Vertex(std::string initialVariable, Vertex* initialTarget, Edge::EdgeType type);


	// add an edge to a pre-constructed target Vertex
	void addTarget(Vertex* targetVertex, Edge::EdgeType type);


	// add an edge to a newly constructed vertex
	void addTarget(std::string targetVar, Edge::EdgeType type);

	std::string getFirstLabel();

	std::vector<Edge> getOutEdges();
};





/////////////////    Graph Class    ////////////////////////
class Graph {
private:
	std::vector<Vertex*> * vertices;

public:
	// to add a pre-constructed vertex to the grapp
	void addVertex(Vertex* newVertex);


	// given a string variable and source and the edge type, this method will
	// construct two new vertices, construct an edge between them, and add the
	// vertices to the graph
	void addVertices(std::string sourceVar, std::string targetVar, Edge::EdgeType type);

	// creates a dot file of the graph for visual inspection
	void createDotFile(std::string fileName);

	Graph();

	std::vector<Vertex*> getVertices() { return vertices;}

};









