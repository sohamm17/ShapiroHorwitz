/*
 * Unionize.h
 *
 *  Created on: Nov 25, 2014
 *      Author: sohamgrad
 */

#ifndef LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_UNIONIZE_H_
#define LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_UNIONIZE_H_

//#include "GraphClasses.h"
#include <vector>
#include <Categorize.h>
#include <string>

class Unionize
{
private:
	std::vector< std::vector<std::string> > categories;

public:
	Unionize(Vertex *a, Vertex *b, std::vector< std::vector<std::string> > categories)
	{
		this->categories = categories;
		std::string variableA = a->getFirstLabel(); //taking any label that belongs to A
		std::string variableB = b->getFirstLabel();

		int i;
		int aFound, bFound, abFound;//false
		for(i = 0; i < this->categories.size(); i++)
		{
			aFound = 0, bFound = 0, abFound = 0;//false
			std::vector<std::string> currentCategory = categories[i];
			int j;

			for(j = 0; j < currentCategory.size() && abFound == 0; j++)
			{
				if(aFound == 0 && currentCategory[j].compare(variableA) == 0)
				{
					aFound = 1;
				}
				else if(aFound == 1 && currentCategory[j].compare(variableB))
				{
					abFound = 1;
				}
				else if(bFound == 0 && currentCategory[j].compare(variableB))
				{
					bFound = 1;
				}
				else if(bFound == 1 && currentCategory[j].compare(variableA))
				{
					abFound = 1;
				}
			}
			//if both of them found break, if any one of them is found
			//because they are in the different category
			if(abFound == 1 || (aFound != bFound))
				break;
		}

		std::vector<Edge *>* unionizedEdgeListPointer;
		if(abFound == 1)
		{
			std::vector<Edge *> * aOutEdgesPointer = a->getOutEdges();
			std::vector<Edge *> aOutEdges = *(aOutEdgesPointer);
			for(i = 0; i < aOutEdges.size(); i++)
				unionizedEdgeListPointer->push_back(aOutEdges[i]);

			std::vector<Edge *> bOutEdges = *(b->getOutEdges());
			for(i = 0; i < bOutEdges.size(); i++)
				unionizedEdgeListPointer->push_back(bOutEdges[i]);

			aOutEdgesPointer->clear();
			aOutEdgesPointer = unionizedEdgeListPointer;

		}
	}
};

//Archieved Code
//std::vector<int>::iterator findA;
//std::vector<int>::iterator findB;
//findA = std::find(currentCategory.begin(), currentCategory.end(), variableA);
//findB = std::find(currentCategory.begin(), currentCategory.end(), variableA);
//
//if(findA != currentCategory.end() && findB != currentCategory.end())
//{
//	abFound = 1;
//	break;
//}
//else if(findA != findB)
//{
//	abFound = 0;
//	break;
//}

#endif /* LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_UNIONIZE_H_ */
