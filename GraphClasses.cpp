//#include <vector>

class Vertex;


/////////////////    Edge class    ///////////////////////
enum EdgeType {
	MAY = 0,
	MUST = 1
};

// no need to manually create edges. Use vertex methods for that
class Edge {
friend class Vertex;
private:
	Vertex * target;
	EdgeType type;

	Edge(Vertex * target, EdgeType type);

};

Edge::Edge(Vertex * target, EdgeType type)
{
	this->target = target;
	this->type = type;

}




/////////////////    Vertex Class   //////////////////////
class Vertex {
private:
	std::vector<Edge*> * outEdges;
	std::vector<std::string> * variables;

public:


	// make a vertex with no outgoing edges
	Vertex(std::string initialVariable);

	// make a vertex with an outgoing edge to a target vertex
	Vertex(std::string initialVariable, Vertex* initialTarget, EdgeType type);


	// add an edge to a pre-constructed target Vertex
	void addTarget(Vertex* targetVertex, EdgeType type);


	// add an edge to a newly constructed vertex
	void addTarget(std::string targetVar, EdgeType type);
};


Vertex::Vertex(std::string initialVariable)
{
	this->outEdges = new std::vector<Edge*>();
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);
}


Vertex::Vertex(std::string initialVariable, Vertex* initialTarget, EdgeType type)
{
	this->outEdges = new std::vector<Edge*>();
	Edge * newEdge = new Edge(initialTarget, type);
	this->outEdges->push_back(newEdge);
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);

}


void Vertex::addTarget(Vertex* targetVertex, EdgeType type)
{
	Edge * outEdge = new Edge(targetVertex, type);
	this->outEdges->push_back(outEdge);
}

void Vertex::addTarget(std::string targetVar, EdgeType type)
{
	Vertex * targetVertex = new Vertex(targetVar);
	Edge * outEdge = new Edge(targetVertex, type);
	this->outEdges->push_back(outEdge);
}



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
	void addVertices(std::string sourceVar, std::string targetVar, EdgeType type);
	Graph();

};

Graph::Graph()
{
	vertices = new std::vector<Vertex*>();
}

void Graph::addVertex(Vertex* newVertex)
{
	vertices->push_back(newVertex);
}

void Graph::addVertices(std::string sourceVar, std::string targetVar, EdgeType type)
{
	Vertex * targetVertex = new Vertex(targetVar);
	Vertex * sourceVertex = new Vertex(sourceVar, targetVertex, type);
	vertices->push_back(sourceVertex);
	vertices->push_back(targetVertex);

}




