#include "GraphClasses.h"
#include <fstream>
#include <iostream>

/////////////////    Edge class    ///////////////////////


Edge::Edge(Vertex * target, EdgeType type)
{
	this->target = target;
	this->type = type;

}

Edge::EdgeType Edge::getType()
{
	return type;
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



// to create a new vertex which is the source. Finds the vertex corresponding
// to the input target label and attaches the new source vertex to it.
// Vertex is added to the graph.
void Graph::createVertex(std::string sourceVar, std::string targetVar, Edge::EdgeType type)
{
	Vertex * targetVertex = (*vertexMap)[targetVar];
	Vertex * newVertex = new Vertex(sourceVar, targetVertex, type);
	(*vertexMap)[sourceVar] = newVertex;
	vertices->push_back(newVertex);



}


void Graph::createVertices(std::string sourceVar, std::string targetVar, Edge::EdgeType type)
{
	Vertex * targetVertex = new Vertex(targetVar);
	(*vertexMap)[targetVar] = targetVertex;

	Vertex * sourceVertex = new Vertex(sourceVar, targetVertex, type);
	(*vertexMap)[sourceVar] = sourceVertex;

	vertices->push_back(sourceVertex);
	vertices->push_back(targetVertex);

}



// given the label of a current vertex and a new label, this method constructs a new
// vertex with the same outgoing edges as the old vertex. Adds to graph
bool Graph::cloneVertex(std::string newLabel, std::string oldLabel)
{
	//TODO
	Vertex * oldVertex = (*vertexMap)[oldLabel];
	if(oldVertex == NULL)
	{
		return false;
		//Throw error
	}
	Vertex * newVertex = new Vertex(newLabel);
	vertices->push_back(newVertex);

	std::vector<Edge *> oldOutEdges = *(oldVertex->getOutEdges());

	unsigned int i;
	for(i = 0; i < oldOutEdges.size(); i++)
	{
		Edge currentEdge = *(oldOutEdges[i]);
		newVertex->addTarget(currentEdge.getTarget(), currentEdge.getType());
	}
	return true;
}



// given two labels correspondng to vertices A and B, this method adds edges from all children of A
// to all children of B
void Graph::storeConnect(std::string a, std::string b){

	Vertex * A = getVertexAtLabel(a);
	Vertex * B = getVertexAtLabel(b);

	fullConnectSets(A->getOutEdges(), B->getOutEdges());


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


Vertex * Graph::getVertexAtLabel(std::string label)
{
	// note: this may need a safety check since apparently at throws an exception
		return vertexMap->at(label);

}


// given two vectors of edge pointers E1 and E2, this method adds edges from the target of edges in E1
// to the target of all edges in E2
void fullConnectSets(std::vector<Edge*> * E1, std::vector<Edge*> * E2)
{
	//todo

}


