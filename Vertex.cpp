/*
 * Vertex.cpp
 *
 *  Created on: Nov 25, 2014
 *      Author: sohamgrad
 */

#include "Vertex.h"
/////////////////    Vertex Class   //////////////////////


Vertex::Vertex(std::string initialVariable)
{
	this->outEdges = new std::vector<Edge*>();
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);
}


Vertex::Vertex(std::string initialVariable, Vertex* initialTarget)
{
	this->outEdges = new std::vector<Edge*>();
	Edge * newEdge = new Edge(initialTarget);
	this->outEdges->push_back(newEdge);
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);

}


void Vertex::addTarget(Vertex* targetVertex)
{
	Edge * outEdge = new Edge(targetVertex);
	this->outEdges->push_back(outEdge);
}

void Vertex::addTarget(std::string targetVar)
{
	Vertex * targetVertex = new Vertex(targetVar);
	Edge * outEdge = new Edge(targetVertex);
	this->outEdges->push_back(outEdge);
}

// given another vertex, this method copies every edge outgoing from the other vertex
// and adds to this one
void Vertex::addTargetsOfOther(Vertex * otherVertex)
{
	std::vector<Edge*>  copyEdges = *(otherVertex->getOutEdges());
	for (int i = 0; i < copyEdges.size(); i++)
	{
		Edge* edgeToCopy = copyEdges[i];
		this->addTarget(edgeToCopy->getTarget());

	}
}

std::string Vertex::getFirstLabel()
{
	return (*variables)[0];
}

std::vector<Edge *> * Vertex::getOutEdges()
{
	return outEdges;
}