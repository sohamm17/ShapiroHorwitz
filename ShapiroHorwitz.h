/*
 * ShapiroHorwitz.h
 *
 *  Created on: Nov 26, 2014
 *      Author: sohamgrad
 */

#ifndef LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_SHAPIROHORWITZ_H_
#define LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_SHAPIROHORWITZ_H_

#include "llvm/IR/Value.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "GraphClasses.h"
#include "Vertex.h"
#include <string>
#include <set>

class ShapiroHorwitz
{
private:
	class tuple
	{
		private:
			std::string source, target;
		public:
			tuple(std::string src, std::string trg)
			{
					source = src;
					target = trg;
			}
			std::string getSource() const { return source; }
			std::string getTarget() const { return target; }
			int compare(const tuple& rhs) const
			{
				if(source.compare(rhs.getSource()) == 0 && target.compare(rhs.getTarget()) == 0)
					return 1;
				return 0;
			}
			void print() const
			{
				llvm::errs() << "(" << source << "," << target << ")";
			}
//			bool operator<(const tuple& rhs) const
//			{
//				return compare(rhs) == 1;
//			}
			bool operator==(const tuple& rhs) const
			{
				return compare(rhs) == 1;
			}
	};

private:
	struct classcomp
	{
		bool operator() (const tuple& lhs, const tuple& rhs) const
		{
			return lhs.compare(rhs) == 0;
		}
	};

	llvm::SetVector<std::string> pointers;
	std::vector< std::map<std::string, int> > categoryValueMap;
	std::vector<ShapiroHorwitz::tuple> *finalPointsToSet;
	Graph * runOnFunction(llvm::Function &F, int run);
	std::set<tuple, classcomp> * getPointsToSetFromGraph(Graph * G);
	std::vector<ShapiroHorwitz::tuple> * getIntersection
		(std::vector< std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * > * allSets);
	void printPointsToIntermediate(std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> mySet);

public:
	//k = number of categories
	//algo can 1 or 2
	ShapiroHorwitz(llvm::SetVector<std::string> Pointers, int k, llvm::Function &F, int algo);
	void printPointsToSet();
	int Alias(const std::string V1, const std::string V2);
	void printPointsToSetSize();
};

#endif /* LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_SHAPIROHORWITZ_H_ */
