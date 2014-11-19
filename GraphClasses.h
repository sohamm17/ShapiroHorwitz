#include <vector>
#include <string>
#include <map>



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

	std::vector<Edge *> * getOutEdges();
};





/////////////////    Graph Class    ////////////////////////
class Graph {
private:
	std::vector<Vertex*> * vertices;
	std::map<std::string, Vertex*> * vertexMap;

public:
	// to create a new vertex which is the source. Finds the vertex corresponding
	// to the input target label and attaches the new source vertex to it.
	// Vertex is added to the graph.
	void createVertex(std::string sourceVar, std::string targetVar, Edge::EdgeType type);


	// given two variables and the edge type (MAY or MUST), this method will
	// construct two new vertices, construct an edge between them, and add the
	// vertices to the graph
	void createVertices(std::string sourceVar, std::string targetVar, Edge::EdgeType type);


	// given the label of a current vertex and a new label, this method constructs a new
	// vertex with the same outgoing edges as the old vertex. Adds to graph
	void cloneVertex(std::string newLabel, std::string oldLabel);

	// creates a dot file of the graph for visual inspection
	void createDotFile(std::string fileName);

	//default constructor. Makes an empty graph
	Graph();

	// getter method
	std::vector<Vertex*> * getVertices() { return vertices;}

};









