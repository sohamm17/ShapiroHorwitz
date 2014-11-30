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
				llvm::errs() << "(" << source << "," << target << ")\n";
			}
			bool operator<(const tuple& rhs) const
			{
				return (source.compare(rhs.getSource()) && target.compare(rhs.getTarget()));
			}
			bool operator==(const tuple& rhs) const
			{
				return compare(rhs);
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

	llvm::SetVector<llvm::Value *> pointers;
	std::vector< std::map<std::string, int> > categoryValueMap;
	std::vector<ShapiroHorwitz::tuple> *finalPointsToSet;
	Graph * runOnFunction(llvm::Function &F, int run);
	std::set<tuple, classcomp> * getPointsToSetFromGraph(Graph * G);
	std::vector<ShapiroHorwitz::tuple> * getIntersection
		(std::vector< std::set<ShapiroHorwitz::tuple, ShapiroHorwitz::classcomp> * > * allSets);

public:
	//k = number of categories
	ShapiroHorwitz(llvm::SetVector<llvm::Value *> Pointers, int k, llvm::Function &F, int algo);
	void printPointsToSet();
};

#endif /* LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_SHAPIROHORWITZ_H_ */
