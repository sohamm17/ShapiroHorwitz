/*
 * ShapiroHorwitz.cpp
 *
 *  Created on: Nov 26, 2014
 *      Author: sohamgrad
 */


#include "ShapiroHorwitz.h"
#include "llvm/IR/Value.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/raw_ostream.h"
#include "Categorize.h"
#include "llvm/IR/Function.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include <string>
#include <set>
#include <climits>
#include <stdio.h>

using namespace llvm;

ShapiroHorwitz::ShapiroHorwitz(SetVector<llvm::Value *> Pointers, int k, Function &F, int algo)
{
	this->pointers = Pointers;
	llvm::shapiro::Categorize<llvm::Value *> categories(Pointers, k);
	int i;
	for(i = 0; i < categories.getNumberOfRuns(); i++)
	{
	  int j;
	  std::map<std::string, int> categoryValueMapPerRun;
	  for(j = 0; j < categories.getNumberOfCategories(); j++)
	  {
		  std::vector<Value *> category = categories.getCategory(j, i);
		  std::vector<std::string> categoryString;
		  int k;
		  for(k = 0; k < (int)category.size(); k++)
		  {
			  std::string o1, o2;
			  {
				  raw_string_ostream os1(o1);
				  WriteAsOperand(os1, (category[k]), true, F.getParent());
			  }
			  categoryValueMapPerRun[o1] = j;
		  }
	  }
	  categoryValueMap.push_back(categoryValueMapPerRun);
	}

	for(i = 0; i < (int)categoryValueMap.size(); i++)
	{
	  errs() << "\nRun " << i + 1 << ":\n";
	  std::map<std::string, int> currentMap = categoryValueMap[i];
	  std::map<std::string, int>::iterator currentMapIterator = currentMap.begin();
	  for(;currentMapIterator != currentMap.end(); currentMapIterator++)
	  {
		  errs() << currentMapIterator->first << "->" << currentMapIterator->second << "\n";
	  }
	}

	int run;
	if(algo == 1)
		run = 1;
	else
		run = categories.getNumberOfRuns();

	std::vector<Graph *> * graphs = new std::vector<Graph *>(run);
	std::vector< std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * > * pointsToSets
			= new std::vector< std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * >(run);

	for(i = 0; i < run; i++)
	{
		(*graphs)[i] = runOnFunction(F, i);
		char* fileName = new char[50];
		sprintf(fileName, "pointsToGraph%d.dot", i);
		errs() << fileName << "\n";
		(*graphs)[i]->createDotFile(fileName);
		(*pointsToSets)[i] = getPointsToSetFromGraph((*graphs)[i]);
	}

	finalPointsToSet = getIntersection(pointsToSets);
}

std::vector<ShapiroHorwitz::tuple> *ShapiroHorwitz::getIntersection
	(std::vector< std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * > * allSets)
{
	std::vector<ShapiroHorwitz::tuple> * intersectedSet = new std::vector<ShapiroHorwitz::tuple>;

	int minSetIndex;
	int i, minSize = std::numeric_limits<int>::max();
	for(i = 0; i < (int) allSets->size(); i++)
	{
		if(minSize > (int) (*allSets)[i]->size())
		{
			minSize = (int) (*allSets)[i]->size();
			minSetIndex = i;
		}
	}

	std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * minSet = (*allSets)[minSetIndex];

	std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp>::iterator minSetIterator = minSet->begin();

	for(; minSetIterator != minSet->end(); minSetIterator++)
	{
		tuple currentTuple = *minSetIterator;
		bool isFound = true;
		for(i = 0; i < (int) allSets->size(); i++)
		{
			if(i != minSetIndex)
			{
				std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp>::iterator findTupleIterator =
						(*allSets)[i]->find(currentTuple);
				if(findTupleIterator == (*allSets)[i]->end())
				{
					isFound = false;
					break;//we didn't find this tuple in one of the sets; so this cannot be in the intersection
				}
			}
		}
		if(isFound) //if it's found in all of the sets then add to the intersected sets
		{
			intersectedSet->push_back(currentTuple);
		}
	}
	return intersectedSet;
}

std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * ShapiroHorwitz::getPointsToSetFromGraph(Graph * G)
{
	std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * pointsToSet
		= new std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp>;
	std::vector<Vertex *> *allVertices = G->getVertices();

	//TODO: Following loop can be optimized
	int i;
	for(i = 0; i < (int) allVertices->size(); i++)
	{
		Vertex *thisVertex = (*allVertices)[i];
		std::vector<Vertex *> *outVertices = thisVertex->getOutVertices();
		std::vector<std::string> *thisVertexLabels = thisVertex->getLabels();
		int j;
		for(j = 0; j < (int) thisVertexLabels->size(); j++)
		{
			std::string thisVertexCurrentLabel = (*thisVertexLabels)[j];
			int k;
			for(k = 0; k < (int) outVertices->size(); k++)
			{
				Vertex *outVertex = (*outVertices)[k];
				std::vector<std::string> *outVertexLabels = outVertex->getLabels();
				int l;
				for(l = 0; l < (int) outVertexLabels->size(); l++)
				{
					std::string outVertexCurrentLabel = (*outVertexLabels)[l];
					tuple currentTuple(thisVertexCurrentLabel, outVertexCurrentLabel);
					pointsToSet->insert(currentTuple);
				}
			}
		}
	}
	return pointsToSet;
}

Graph * ShapiroHorwitz::runOnFunction(llvm::Function &F, int run)
{
	Graph * pointsToGraph = new Graph(&categoryValueMap[run]);

	int StackCounter = 1, HeapCounter = 1;

	for(Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)
	{
		for (BasicBlock::iterator I = BI->begin(), E = BI->end(); I != E; ++I)
		{
			std::string o1, o2;
			if (LoadInst *AI = dyn_cast<LoadInst>(I))
			{
				//errs() << "Load: " << *I << "\n";
				{
					  raw_string_ostream os1(o1), os2(o2);
					  WriteAsOperand(os1, AI, true, F.getParent());
					  WriteAsOperand(os2, AI -> getOperand(0), true, F.getParent());
				}
				pointsToGraph->loadConnect(o2, o1);
				//WriteAsOperand(errs(), AI->getOperand(0), true, F.getParent());
				//errs() << o1 << "\t" << o2 << "\n";
				//errs() << "\n\n";
			}
			else if(StoreInst *AI = dyn_cast<StoreInst>(I))
			{
//				errs() << "Store: " << *I << "\n";
				{
					  raw_string_ostream os1(o1), os2(o2);
					  WriteAsOperand(os1, AI->getOperand(0), true, F.getParent());
					  WriteAsOperand(os2, AI->getOperand(1), true, F.getParent());
				}

				if(Type *firstOperandType = dyn_cast<PointerType>(AI->getOperand(0)->getType()))
				{
//					errs() << "Operand1:" << o1 << ":" << *AI->getOperand(0)->getType();
//					errs() << "\tOperand2:" << o2 << ":" << *AI->getOperand(1)->getType();
//					errs() << "\n\n";
					pointsToGraph->storeConnect(o2, o1);
				}
			}
			else if(AllocaInst *AI = dyn_cast<AllocaInst>(I))
			{
				{
					  raw_string_ostream os1(o1);
					  WriteAsOperand(os1, AI, true, F.getParent());
				}

				char* stackLoc = new char[50];
				sprintf(stackLoc, "Stack%d", StackCounter++);
				//errs() << "Alloca: " << o1 << "\n";
				pointsToGraph->createVertices(o1, stackLoc);
				//errs() << "Alloca: " << o1 << "\n";

				//pointsToGraph.clone(o1, o2, Edge::MAY);
				//errs() << "BitCast: " << o1 << "\t" << o2 << "\n";
			}
			else if(BitCastInst *AI = dyn_cast<BitCastInst>(I))
			{
				{
					  raw_string_ostream os1(o1), os2(o2);
					  WriteAsOperand(os1, AI, true, F.getParent());
					  WriteAsOperand(os2, AI->getOperand(0), true, F.getParent());
				}


				if(!pointsToGraph->cloneVertex(o1, o2))
					;//errs() << "BitCast: Clonning problem:\t" << o2 << "\t" << o1 << "\n";
				else
				{
//						Unionize<std::string> myUnion(pointsToGraph.getVertexAtLabel(o1),
//								pointsToGraph.getVertexAtLabel(o2), categoriesLastRun);
					//errs() << "BitCast: Clonning happened:\t" << o2 << "\t" << o1 << "\n";
				}
				//errs() << "BitCast: " << o1 << "\t" << o2 << "\n";
			}
			else if(CallInst *AI = dyn_cast<CallInst>(I))
			{
				if(AI->getCalledFunction()->getName().compare_lower("malloc") == 0)
				{
					{
						raw_string_ostream os1(o1);
						WriteAsOperand(os1, AI, true, F.getParent());
					}
					//errs() << "LHS: " << o1 << "\n";
					char* heapLoc = new char[50];
					sprintf(heapLoc, "Malloc%d", HeapCounter++);
					pointsToGraph->createVertices(o1, heapLoc);
				}
			}
		}
	}
	return pointsToGraph;
}

void ShapiroHorwitz::printPointsToSet()
{
	std::vector<ShapiroHorwitz::tuple>::iterator finalPointsToSetIteraor = finalPointsToSet->begin();
	for(; finalPointsToSetIteraor != finalPointsToSet->end(); finalPointsToSetIteraor++)
	{
		tuple curTuple = (*finalPointsToSetIteraor);
		errs() << "(" << curTuple.getSource() << "," << curTuple.getTarget() << ")";
		errs() << "  ";
	}
	errs() << "\n";
}

