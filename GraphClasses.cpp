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

	// check if source already exists
	Vertex * sourceVertex = getVertexAtLabel(sourceVar);
	if (sourceVertex == NULL)
	{
		Vertex * newVertex = new Vertex(sourceVar, targetVertex);
		(*vertexMap)[sourceVar] = newVertex;
		vertices->push_back(newVertex);
	}
	else
	{
		createEdge(sourceVertex, targetVertex);
	}


}


// to create a new vertex without edges and add it to the graph.
Vertex * Graph::createVertex(std::string vertexLabel)
{
	// check if already exists
	Vertex * newVertex = getVertexAtLabel(vertexLabel);
	if (newVertex == NULL)
	{
		newVertex = new Vertex(vertexLabel);
		(*vertexMap)[vertexLabel] = newVertex;
		vertices->push_back(newVertex);
	}


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
	delete thisVertex;
}



void Graph::createVertices(std::string sourceVar, std::string targetVar)
{
	createVertex(targetVar);

	createVertex(sourceVar, targetVar);

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
	//llvm::errs() << "1.In AddTargetsOfOther:" << this->vertices->size() << "\n";
	for (int i = 0; i < copyEdges.size(); i++)
	{
		Edge* edgeToCopy = copyEdges[i];
		Vertex * targetVertex = edgeToCopy->getTarget();
		//llvm::errs() << "1.5. In For Loop AddTargetsOfOther:" << targetVertex->getFirstLabel() << "\n";

		if (!thisVertex->alreadyHasEdge(targetVertex))
		{
			//llvm::errs() << "1.6. In If Cond AddTargetsOfOther:" << targetVertex->getFirstLabel() << "\n";
			createEdge(thisVertex, targetVertex);
			//llvm::errs() << "1.7. End If Cond AddTargetsOfOther:" << targetVertex->getFirstLabel() << "\n";
		}
	}
	//llvm::errs() << "2.End AddTargetsOfOther:" << this->vertices->size() << "\n";
}

// given another vertex, this method copies every edge incoming to the other vertex
// and adds to this one
void Graph::addSourcesOfOther(Vertex * thisVertex, Vertex * otherVertex)
{
	//std::cout << "thisvertex " << thisVertex->toString() << "\n";
	//std::cout << "otherVertex " << otherVertex->toString() << "\n";

	std::vector<Edge*>  copyEdges = *(otherVertex->getInEdges());
	for (int i = 0; i < copyEdges.size(); i++)
	{
		Edge* edgeToCopy = copyEdges[i];
		//thisVertex->addTarget(edgeToCopy->getTarget());
		edgeToCopy->getSource()->addTarget(thisVertex); // no need to be recursive
		//createEdge(edgeToCopy->getSource(), thisVertex);
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

// given two strings corresponding to existing vertices, and a label for a
// to-be-constructed vertex, this method creates the vertex with all the children
// of each vertex
// Called to process "phi" calls in LLVM
void Graph::phiConnect(std::string newLabel, std::string a, std::string b)
{
	//llvm::errs() << "1.In Phi:" << this->vertices->size() << "\n";
	Vertex * aVertex = createVertex(a);
	//llvm::errs() << "2.In Phi:" << this->vertices->size() << "\n";
	Vertex * bVertex = createVertex(b);
	//llvm::errs() << "3.In Phi:" << this->vertices->size() << "\n";
	Vertex * newVertex = createVertex(newLabel);
	//llvm::errs() << "4.In Phi:" << this->vertices->size() << "\n";
	addTargetsOfOther(newVertex, aVertex);
	addTargetsOfOther(newVertex, bVertex);
	//llvm::errs() << "5.In Phi:" << this->vertices->size() << "\n";
}


// given the label for a new variable, this method creates a new vertex with an edge
// to all other vertices. This is called for non-malloc functions (very conservative)
void Graph::functionConnect(std::string newLabel)
{
	Vertex * newVertex = createVertex(newLabel);
	std::vector<Vertex *>::iterator vi = this->vertices->begin();
	for (; vi != this->vertices->end(); vi++)
	{
		createEdge(newVertex, (*vi));
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
	/*if (vertexMap->find( label ) != vertexMap->end())
		return vertexMap->at(label);
	else
		return NULL;
	*/
	return (*vertexMap)[label];
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
			merge(currentChild, target);
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

	if (categoryMap == NULL)
	{
		// default behaviour is steensgard (all in same category)
		return true;
	}
	int categoryA = categoryMap->at(labelA);
	int categoryB = categoryMap->at(labelB);
	//std::cout << "category of " << labelA << " == " << categoryA;

	return (categoryA == categoryB);


}

// given vertices A and B, this method merges vertex B into vertex A. Vertex A will now have all
// incoming and outgoing edges that B had. B is removed from the graph.
void Graph::merge(Vertex * A, Vertex* B)

{
	std::cout << "merge " << A->getFirstLabel() << " and " << B->getFirstLabel() << "\n";
	addTargetsOfOther(A, B);
	addSourcesOfOther(A,B);
	takeLabels(A,B);

	std::cout << " removing " << B->getFirstLabel() << "\n";
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




