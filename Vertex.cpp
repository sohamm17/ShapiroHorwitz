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
	this->inEdges = new std::vector<Edge*>();
	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);
}


Vertex::Vertex(std::string initialVariable, Vertex* initialTarget)
{
	this->outEdges = new std::vector<Edge*>();
	this->inEdges = new std::vector<Edge*>();
//	Edge * newEdge = new Edge(this, initialTarget);
//	this->outEdges->push_back(newEdge);
//
//	//adding this edge to the in-edge list of initialTarget vertex
//	std::vector<Edge *> * initialTargetInEdges = initialTarget->getInEdges();
//	initialTargetInEdges->push_back(newEdge);

	this->variables = new std::vector<std::string>();
	this->variables->push_back(initialVariable);

	this->addTarget(initialTarget);
}

Vertex::~Vertex()
{
	int i;
	for(i = 0; i < (int) inEdges->size(); i++)
	{
		Vertex * source = (*inEdges)[i]->getSource();
		source->removeInEdge((*inEdges)[i]);
		delete[] (*inEdges)[i];
	}

	for(i = 0; i < (int) outEdges->size(); i++)
	{
		Vertex * target = (*outEdges)[i]->getTarget();
		target->removeOutEdge((*outEdges)[i]);
		delete[] (*outEdges)[i];
	}

	delete[] inEdges;
	delete[] outEdges;
	delete[] variables;
}

void Vertex::removeInEdge(Edge * edge)
{
	std::vector<Edge *>::iterator inEdgesIterator = inEdges->begin();
	int i = 0;
	for(; inEdgesIterator != inEdges->end(); inEdgesIterator++, i++)
	{
		if(*inEdgesIterator == edge)
			break;
	}

	inEdges->erase(inEdges->begin() + i);
}

void Vertex::removeOutEdge(Edge * edge)
{
	std::vector<Edge *>::iterator inEdgesIterator = outEdges->begin();
	int i = 0;
	for(; inEdgesIterator != outEdges->end(); inEdgesIterator++, i++)
	{
		if(*inEdgesIterator == edge)
			break;
	}

	outEdges->erase(outEdges->begin() + i);
}

void Vertex::addTarget(Vertex* targetVertex)
{
	Edge * outEdge = new Edge(this, targetVertex);
	this->outEdges->push_back(outEdge);
	targetVertex->getInEdges()->push_back(outEdge);
}

void Vertex::addTarget(std::string targetVar)
{
	Vertex * targetVertex = new Vertex(targetVar);
	this->addTarget(targetVertex);
//	Edge * outEdge = new Edge(targetVertex);
//	this->outEdges->push_back(outEdge);
}

// given another vertex, this method copies every edge outgoing from the other vertex
// and adds to this one
void Vertex::addTargetsOfOther(Vertex * otherVertex)
{
	std::vector<Edge*> copyEdges = *(otherVertex->getOutEdges());
	for (int i = 0; i < (int) copyEdges.size(); i++)
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

std::vector<Edge *> * Vertex::getInEdges()
{
	return inEdges;
}

std::vector<Vertex *> * Vertex::getOutVertices()
{
	std::vector<Vertex *> * outVertices = new std::vector<Vertex *>();
	std::vector<Edge *>::iterator outIterator = outEdges->begin();
	for(; outIterator != outEdges->end(); outIterator++)
	{
		outVertices->push_back((*outIterator)->getTarget());
	}
	return outVertices;
}

std::vector<std::string> * Vertex::getLabels()
{
	return variables;
}

void Vertex::takeLabels(Vertex * other)
{
	std::vector<std::string> * otherVariables = other->getLabels();
	std::vector<std::string>::iterator labelIterator = otherVariables->begin();
	for(; labelIterator != otherVariables->end(); labelIterator++)
	{
		this->variables->push_back(*labelIterator);
	}
}
