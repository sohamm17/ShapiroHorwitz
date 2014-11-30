#include "GraphClasses.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>


/////////////////    Graph Class    ////////////////////////


Graph::Graph(std::map<std::string, int> * categoryMap)
{
	vertices = new std::vector<Vertex*>();
	vertexMap = new std::map<std::string, Vertex*>;
	this->categoryMap = categoryMap;
}



// to create a new vertex which is the source. Finds the vertex corresponding
// to the input target label and attaches the new source vertex to it.
// Vertex is added to the graph.
void Graph::createVertex(std::string sourceVar, std::string targetVar)
{
	Vertex * targetVertex = (*vertexMap)[targetVar];
	Vertex * newVertex = new Vertex(sourceVar, targetVertex);
	(*vertexMap)[sourceVar] = newVertex;
	vertices->push_back(newVertex);



}


// to create a new vertex without edges and add it to the graph.
Vertex * Graph::createVertex(std::string vertexLabel)
{
	Vertex * newVertex = new Vertex(vertexLabel);
	(*vertexMap)[vertexLabel] = newVertex;
	vertices->push_back(newVertex);
	return newVertex;
}


void Graph::removeVertex(std::string vertexLabel)
{
	Vertex * thisVertex = (*vertexMap)[vertexLabel];
	if (thisVertex == NULL)
	{
		return;
	}

	else
	{
		removeVertex(thisVertex);
		return;
	}

}

void Graph::removeVertex(Vertex * thisVertex)
{
	int i;
	for (i = 0; i < vertices->size(); i++)
	{
		if ((*vertices)[i] == thisVertex)
		{
			// we found the vertex
			break;
		}
	}
	vertices->erase(vertices->begin() + i);
	delete[] thisVertex;
}



void Graph::createVertices(std::string sourceVar, std::string targetVar)
{
	Vertex * targetVertex = new Vertex(targetVar);
	(*vertexMap)[targetVar] = targetVertex;

	Vertex * sourceVertex = new Vertex(sourceVar, targetVertex);
	(*vertexMap)[sourceVar] = sourceVertex;

	vertices->push_back(sourceVertex);
	vertices->push_back(targetVertex);

}


// given labels corresponding to vertices, this method adds an edge from the source
// to the target
bool Graph::createEdge(std::string sourceVar, std::string targetVar)
{
	Vertex * sourceVertex = (*vertexMap)[sourceVar];
	Vertex * targetVertex = (*vertexMap)[targetVar];
	if(sourceVertex == NULL || targetVertex == NULL)
	{
		return false;
		//Throw error
	}

	//sourceVertex->addTarget(targetVertex);
	createEdge(sourceVertex, targetVertex);
	return true;

}

// given vertices, this method adds an edge from the source
// to the target
void Graph::createEdge(Vertex * sourceVertex, Vertex * targetVertex)
{
	if (unionize(sourceVertex, targetVertex))
	{
		// the target vertex was merged with one of the sources's children
		return;
	}
	else
	{
		// no merge happened so add the target as normal
		sourceVertex->addTarget(targetVertex);
		return;
	}

}


// given the label of a current vertex and a new label, this method constructs a new
// vertex with the same outgoing edges as the old vertex. Adds to graph
bool Graph::cloneVertex(std::string newLabel, std::string oldLabel)
{

	Vertex * oldVertex = (*vertexMap)[oldLabel];
	if(oldVertex == NULL)
	{
		return false;
		//Throw error
	}
	Vertex * newVertex = createVertex(newLabel);
	//vertices->push_back(newVertex);

	std::vector<Edge *> oldOutEdges = *(oldVertex->getOutEdges());

	unsigned int i;
	for(i = 0; i < oldOutEdges.size(); i++)
	{
		Edge currentEdge = *(oldOutEdges[i]);
		newVertex->addTarget(currentEdge.getTarget());
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


// given another vertex, this method copies every edge outgoing from the other vertex
// and adds to this one
void Graph::addTargetsOfOther(Vertex * thisVertex, Vertex * otherVertex)
{
	std::vector<Edge*>  copyEdges = *(otherVertex->getOutEdges());
	for (int i = 0; i < copyEdges.size(); i++)
	{
		Edge* edgeToCopy = copyEdges[i];
		//thisVertex->addTarget(edgeToCopy->getTarget());
		createEdge(thisVertex, edgeToCopy->getTarget());
	}
}

// given another vertex, this method copies every edge incoming to the other vertex
// and adds to this one
void Graph::addSourcesOfOther(Vertex * thisVertex, Vertex * otherVertex)
{
	std::vector<Edge*>  copyEdges = *(otherVertex->getInEdges());
	for (int i = 0; i < copyEdges.size(); i++)
	{
		Edge* edgeToCopy = copyEdges[i];
		//thisVertex->addTarget(edgeToCopy->getTarget());
		createEdge(edgeToCopy->getSource(), thisVertex);
	}
}

// given two labels correspondng to existing vertex A and to-be-constructed vetex B,
// this method creates B with outgoing edges copied from all children of A
// Called to process "load" calls in LLVM
void Graph::loadConnect(std::string a, std::string b)
{
	Vertex * A = getVertexAtLabel(a);
	Vertex * B = createVertex(b);
	std::vector<Edge *> edges = *(A->getOutEdges());
	for (int i = 0; i < edges.size(); i++)
	{
		Edge * currentEdge = edges[i];
		Vertex * currentVertex = currentEdge->getTarget();
		addTargetsOfOther(B, currentVertex);
	}
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
//		llvm::errs() << "i = " << i << "\n";
		Vertex* currentVertex = (*vertices)[i];
		if(currentVertex != NULL)
		{
			std::vector<Edge*>* currentEdges = currentVertex->getOutEdges();

//			llvm::errs() << currentVertex->toString() << "\n";
			for (int j = 0; j < (int) currentEdges->size(); j++)
			{
//				llvm::errs() << "j= " << j << "\n";
				Edge* currentEdge = (*currentEdges)[j];
				if(currentEdge != NULL)
				{
//					llvm::errs() << "NOT NULL\n";
					outFile << "\"" << currentVertex->toString() << "\"" << " -> "
						<< "\"" << currentEdge->getTarget()->toString() << "\"" << "\n";
				}
//				else
//					llvm::errs() << "NULL Found\n";
//				llvm::errs() << "I am here.\n";
			}
		}
//		else
//			llvm::errs() << "NULL vertex";
//		llvm::errs() << "I am out of inner loop.\n";
//		llvm::errs() << "Vertices size:" << vertices->size() << ".\n";
	}

	outFile << "}\n";

//	llvm::errs() << "createDotFile\n";
	outFile.close();




}


Vertex * Graph::getVertexAtLabel(std::string label)
{
	// note: this may need a safety check since apparently at throws an exception
		return vertexMap->at(label);

}


// given two vectors of edge pointers E1 and E2, this method adds edges from the target of edges in E1
// to the target of all edges in E2
void Graph::fullConnectSets(std::vector<Edge*> * E1, std::vector<Edge*> * E2)
{

	std::vector<Edge*> sourceEdges = *E1;
	std::vector<Edge*> targetEdges = *E2;

	for (int i = 0; i < sourceEdges.size(); i++)
	{
		Edge * currentSourceEdge = sourceEdges[i];
		Vertex * currentSourceVertex = currentSourceEdge->getTarget();
		for (int j = 0; j < targetEdges.size(); j++)
		{
			Edge * currentTargetEdge = targetEdges[j];
			Vertex * currentTargetVertex = currentTargetEdge->getTarget();
			//currentSourceVertex->addTarget(currentTargetVertex);
			createEdge(currentSourceVertex, currentTargetVertex);

		}
	}

}

// given a source vertex and a target vertex ,
// this method checks to see if the target needs to be unionized with any of the source's children (at most one),
// and if so it merges them into the other vertex. All vertices that point to the target vertex will have their target
// updated, and the target vertex is removed from the graph
// return true if a merge hapened, and false if no merge occurred
bool Graph::unionize(Vertex * source, Vertex * target)
{
	//TODO:: this is currently segfaulting!!
	std::vector<Vertex *> children = *(source->getOutVertices());
	for (int i = 0; i < children.size(); i++)
	{
		Vertex * currentChild = children[i];
		if (isSameCategory(currentChild, target))
		{
			//merge(currentChild, target);
			return true; // only possible to merge with one child
		}

	}

	return false;
}

bool Graph::isSameCategory(Vertex * A, Vertex * B)
{
	// we only need the first label from the vertices since every label of a given
	// vertex must be in the same category
	std::string labelA = A->getFirstLabel();
	std::string labelB = B->getFirstLabel();

	int categoryA = (*categoryMap)[labelA];
	int categoryB = (*categoryMap)[labelB];

	return (categoryA == categoryB);

	return true;

}

// given vertices A and B, this method merges vertex B into vertex A. Vertex A will now have all
// incoming and outgoing edges that B had. B is removed from the graph.
void Graph::merge(Vertex * A, Vertex* B)

{
	addTargetsOfOther(A, B);
	addSourcesOfOther(A,B);
	takeLabels(A,B);
	removeVertex(B);
	return;

}

// given vertixes A and B, this method takes all the labels from B and adds them to the list of A's labels.
// it also updates the vertexMap with this new information
void Graph::takeLabels(Vertex * A, Vertex * B)
{
	std::vector<std::string> * labels = B->getLabels();
	std::vector<std::string>::iterator labelIterator = labels->begin();
	for(; labelIterator != labels->end(); labelIterator++)
	{
		std::string currentLabel = (*labelIterator);
		(*vertexMap)[currentLabel] = A; // update the map for each label from B

	}

	A->takeLabels(B);

	return;
}




