#include "GraphClasses.h"
#include <fstream>
#include <iostream>

/////////////////    Edge class    ///////////////////////


Edge::Edge(Vertex * target, EdgeType type)
{
	this->target = target;
	this->type = type;

}

Vertex* Edge::getTarget()
{
	return target;
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

std::string Vertex::getFirstLabel()
{
	return (*variables)[0];
}

std::vector<Edge *> * Vertex::getOutEdges()
{
	return outEdges;
}
/////////////////    Graph Class    ////////////////////////


Graph::Graph()
{
	vertices = new std::vector<Vertex*>();
	vertexMap = new std::map<std::string, Vertex*>;
}

void Graph::createVertex(std::string varLabel)
{
	Vertex * newVertex = new Vertex(varLabel);
	vertexMap[varLabel] = newVertex;
	vertices->push_back(newVertex);
}

void Graph::createVertices(std::string sourceVar, std::string targetVar, Edge::EdgeType type)
{
	Vertex * targetVertex = new Vertex(targetVar);
	vertexMap[targetVar] = targetVertex;

	Vertex * sourceVertex = new Vertex(sourceVar, targetVertex, type);
	vertexMap[sourceVar] = sourceVertex;

	vertices->push_back(sourceVertex);
	vertices->push_back(targetVertex);

}


void Graph::createDotFile(std::string fileName)
{
	std::ofstream outFile;
	outFile.open(fileName.c_str());

	outFile << "digraph {\n";
	outFile << "ordering=out;\nranksep=.4;\n  bgcolor=\"lightgrey\"; node [shape=box, fixedsize=false, fontsize=12, fontname=\"Helvetica-bold\", fontcolor=\"blue\"\n";
    outFile << "width=.25, height=.25, color=\"black\", fillcolor=\"white\", style=\"filled, solid, bold\"];\n";
	outFile << "edge [arrowsize=.5, color=\"black\", style=\"bold\"];\n";

	for (int i = 0; i < vertices->size(); i++)
	{
		Vertex currentVertex = *((*vertices)[i]);
		std::vector<Edge*> currentEdges = *(currentVertex.getOutEdges());

		for (int j = 0; j < currentEdges.size(); j++)
		{
			Edge currentEdge = *(currentEdges[j]);
			outFile << "\"" << currentVertex.getFirstLabel() << "\"" << " -> "
					<< "\"" << currentEdge.getTarget()->getFirstLabel() << "\"" << "\n";

		}
	}

	outFile << "}\n";

	outFile.close();




}
