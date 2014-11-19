#include "GraphClasses.h"
#include <vector>

/////////////////    Edge class    ///////////////////////


Edge::Edge(Vertex * target, EdgeType type)
{
	this->target = target;
	this->type = type;

}




/////////////////    Vertex Class   //////////////////////


Vertex::Vertex(std::string initialVariable)
{
	this->outEdges = new std::vector<Edge*>();
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);
}


Vertex::Vertex(std::string initialVariable, Vertex* initialTarget, Edge::EdgeType type)
{
	this->outEdges = new std::vector<Edge*>();
	Edge * newEdge = new Edge(initialTarget, type);
	this->outEdges->push_back(newEdge);
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);

}


void Vertex::addTarget(Vertex* targetVertex, Edge::EdgeType type)
{
	Edge * outEdge = new Edge(targetVertex, type);
	this->outEdges->push_back(outEdge);
}

void Vertex::addTarget(std::string targetVar, Edge::EdgeType type)
{
	Vertex * targetVertex = new Vertex(targetVar);
	Edge * outEdge = new Edge(targetVertex, type);
	this->outEdges->push_back(outEdge);
}



/////////////////    Graph Class    ////////////////////////


Graph::Graph()
{
	vertices = new std::vector<Vertex*>();
}

void Graph::addVertex(Vertex* newVertex)
{
	vertices->push_back(newVertex);
}

void Graph::addVertices(std::string sourceVar, std::string targetVar, Edge::EdgeType type)
{
	Vertex * targetVertex = new Vertex(targetVar);
	Vertex * sourceVertex = new Vertex(sourceVar, targetVertex, type);
	vertices->push_back(sourceVertex);
	vertices->push_back(targetVertex);

}




