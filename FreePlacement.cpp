//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "freeplacement"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/InitializePasses.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Assembly/Writer.h"
#include "GraphClasses.h"
#include <string>
#include <stdio.h>

using namespace llvm;

STATISTIC(FreeCounter, "Counts number of pointers");

namespace {
// Hello - The first implementation, without getAnalysisUsage.
struct FreePlacement : public FunctionPass {
	static char ID; // Pass identification, replacement for typeid
	Graph pointsToGraph;
	FreePlacement() : FunctionPass(ID) {}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const
	{
		//AU.addRequired<AliasAnalysis>();
		//AU.setPreservesAll();
	}

	virtual bool runOnFunction(Function &F)
	{
		++FreeCounter;
		errs() << "Free Placement:" << F.getName() << "\n";
		std::vector<ReturnInst*> Returns;
		std::vector<CallInst*> Frees;

		int StackCounter = 1, HeapCounter = 1;

		// Funtion iterator returns an iterator which iterates through the basic block
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
					//WriteAsOperand(errs(), AI->getOperand(0), true, F.getParent());
					//errs() << o1 << "\t" << o2 << "\n";
					//errs() << "\n\n";
				}
				else if(StoreInst *AI = dyn_cast<StoreInst>(I))
				{
					errs() << "Store: " << *I << "\n";
					{
						  raw_string_ostream os1(o1), os2(o2);
						  WriteAsOperand(os1, AI->getOperand(0), true, F.getParent());
						  WriteAsOperand(os2, AI->getOperand(1), true, F.getParent());
					}
					if(Type *firstOperandType = dyn_cast<PointerType>(AI->getOperand(0)->getType()))
					{
						errs() << "Operand0:" << o1 << ":" << *AI->getOperand(0)->getType();
						errs() << "\tOperand1:" << o2 << ":" << *AI->getOperand(1)->getType();
						errs() << "\n\n";
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
					pointsToGraph.createVertices(o1, stackLoc, Edge::MAY);
					errs() << "Alloca: " << o1 << "\n";

					//pointsToGraph.clone(o1, o2, Edge::MAY);
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
						errs() << "LHS: " << o1 << "\n";
						char* heapLoc = new char[50];
						sprintf(heapLoc, "Malloc%d", HeapCounter++);
						pointsToGraph.createVertices(o1, heapLoc, Edge::MUST);
					}
				}
			}
		}

		pointsToGraph.createDotFile("pointsToGraph.dot");

//		for(Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)
//		{
//			for (BasicBlock::iterator I = BI->begin(), E = BI->end(); I != E; ++I)
//			{
//				for(Function::iterator BI2 = F.begin(), BE2 = F.end(); BI2 != BE2; ++BI2)
//				{
//					for (BasicBlock::iterator I2 = BI2->begin(), E2 = BI2->end(); I2 != E2; ++I2)
//					{
//						//Instruction *AI, *AI2;
//						//if ((AI = dyn_cast<Instruction>(I)) && (AI2 = dyn_cast<Instruction>(I2)))
//						{
//							errs() << *I << ", " << *I2 << ":\t";
//							errs() << AA->alias(I, I2) << "\n"; //!= AliasAnalysis::NoAlias)
//
//						}
//					}
//				}
//			}
//		}
		return false;
	}
};
}


char FreePlacement::ID = 0;
static RegisterPass<FreePlacement> X("FreePlacement", "Free Placement Pass");
