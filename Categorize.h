/*
 * Categorize.h
 *
 *  Created on: Nov 22, 2014
 *      Author: sohamgrad
 */

#ifndef LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_CATEGORIZE_H_
#define LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_CATEGORIZE_H_

#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Assembly/Writer.h"
#include "Base.h"

namespace llvm {
namespace shapiro {
template<typename T>
void printArr(std::vector<T> arr)
{
	//std::cout << "[ ";
	llvm::errs() << "[ ";
	int i;
	for(i = 0; i < (int) arr.size(); i++)
		llvm::errs() << arr[i] << " ";
	llvm::errs() << "]\n";
}

template <class T>
class Categorize
{
	private:
		typedef typename std::vector<T> templateVector;
		typedef typename llvm::SetVector<T>::iterator setVectoriterator;

		std::vector<Base> arr;
		std::map<Base*, T> varToBaseMap;
		std::vector< templateVector > categoryBucket;
		int numVariables;
		int numberRuns;
		int numCategories;

		void generateArray(int start, int end, int base)
		{
			//Base **arr = (Base**) malloc(sizeof(Base *) * (end-start));
			int i, j = 0;
			for(i = start - 1; i < end; i++, j++)
			{
				arr.push_back(*(new Base(base, i)));
			}
		}

	public:

		Categorize(llvm::SetVector<T> collection, int category)
		{
			//llvm::errs() << "In Constructor\n";
			numVariables = (int) collection.size();
			numCategories = category;
			this->generateArray(1, numVariables, numCategories);
			//llvm::errs() << "Created Base array\n";
			categoryBucket = *(new std::vector< templateVector >(category));
			//llvm::errs() << "Created Category Bucket\n";
			numberRuns = 0;
			// tricky way to calculate the number of runs
			numberRuns = (int) arr[numVariables - 1].getFullNumber().size();

			//	Mathematical way to calculate number of runs
			//	float logkN = (float)(log10(numVariables) / log10(category));
			//
			//	if(logkN > (int)logkN)
			//		numberRuns = (int) (logkN) + 1;
			//	else
			//		numberRuns = (int) logkN;
//			for(i = 0; i < numberRuns; i++)
//			{
//				int j;
//				cout << "\nIteration :" << i + 1 << endl;
//				for(j = 0; j < category; j++)
//				{
//					int k;
//					categoryBucket[j].clear();
//					for(k = 0; k < numVariables; k++)
//					{
//						if(arr[k]->getNumberAtIndex(i) == j)
//							categoryBucket[j].push_back(k);
//					}
//					cout << j << ":";
//					printArr(categoryBucket[j]);
//				}
//			}
			int i = 0;
			for (setVectoriterator I1 = collection.begin(), E = collection.end();I1 != E; ++I1)
			{
				varToBaseMap.insert(std::pair<Base*, T>((&arr[i++]), *I1) );
			}
			//llvm::errs() << "End of Constructor\n";
		}

		std::vector<T> getCategory(int category, int run)
		{
			//cout << "\nIteration :" << i + 1 << endl;
			int k;
			//llvm::errs() << "In the getCategory: " << category << " " << run << "\n";
			categoryBucket[category].clear();
			for(k = 0; k < numVariables; k++)
			{
				Base* currentBase = &arr[k];
				if(currentBase->getNumberAtIndex(run) == category)
				{
					T currentMappedTemplate = varToBaseMap[currentBase];
					categoryBucket[category].push_back(currentMappedTemplate);
				}
			}
			//cout << j << ":";
			//printArr<T>(categoryBucket[category]);
			return categoryBucket[category];
		}
		int getNumberOfRuns()
		{
			return this->numberRuns;
		}
		int getNumberOfCategories()
		{
			return this->numCategories;
		}
};

}//shapiro
}//llvm
#endif /* LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_CATEGORIZE_H_ */
