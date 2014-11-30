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
#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/InstIterator.h"
//#include "GraphClasses.h"
#include "Categorize.h"
#include "ShapiroHorwitz.h"
//#include "Unionize.h"
#include <string>
#include <stdio.h>

using namespace llvm;

STATISTIC(FreeCounter, "Counts number of pointers");

namespace {
// Hello - The first implementation, without getAnalysisUsage.
struct FreePlacement : public FunctionPass {
	static char ID; // Pass identification, replacement for typeid
	std::vector< std::map<std::string, int> > categoryValueMap;
	FreePlacement() : FunctionPass(ID) {}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const
	{
		//AU.addRequired<AliasAnalysis>();
		//AU.setPreservesAll();
	}

	static inline bool isInterestingPointer(Value *V) {
	  return V->getType()->isPointerTy()
	      && !isa<ConstantPointerNull>(V);
	}

	void getPointersCategorized(Function &F, int k)
	{
		  SetVector<Value *> Pointers;
		  SetVector<CallSite> CallSites;
		  //SetVector<Value *> Loads;
		  //SetVector<Value *> Stores;

		  for (Function::arg_iterator I = F.arg_begin(), E = F.arg_end(); I != E; ++I)
		    if (I->getType()->isPointerTy())    // Add all pointer arguments.
		      Pointers.insert(I);

		  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
		    if (I->getType()->isPointerTy()) // Add all pointer instructions.
		      Pointers.insert(&*I);
//		    if (EvalTBAA && isa<LoadInst>(&*I))
//		      Loads.insert(&*I);
//		    if (EvalTBAA && isa<StoreInst>(&*I))
//		      Stores.insert(&*I);
		    Instruction &Inst = *I;
		    if (CallSite CS = cast<Value>(&Inst)) {
		      Value *Callee = CS.getCalledValue();
		      // Skip actual functions for direct function calls.
		      if (!isa<Function>(Callee) && isInterestingPointer(Callee))
		        Pointers.insert(Callee);
		      // Consider formals.
		      for (CallSite::arg_iterator AI = CS.arg_begin(), AE = CS.arg_end();
		           AI != AE; ++AI)
		        if (isInterestingPointer(*AI))
		          Pointers.insert(*AI);
		      CallSites.insert(CS);
		    } else {
		      // Consider all operands.
		      for (Instruction::op_iterator OI = Inst.op_begin(), OE = Inst.op_end();
		           OI != OE; ++OI)
		        if (isInterestingPointer(*OI))
		          Pointers.insert(*OI);
		    }
		  }

		  for (SetVector<Value *>::iterator I1 = Pointers.begin(), E = Pointers.end();
		         I1 != E; ++I1)
		  {
				std::string o1, o2;
				{
					  raw_string_ostream os1(o1), os2(o2);
					  WriteAsOperand(os1, *I1, true, F.getParent());
				}
				errs() << o1 << "\n";
		  }


		  //llvm::shapiro::Categorize<Value *> categories(Pointers, k);

		  //errs() << "Back to LLVM after constructor";
		  int i;
		  /*for(i = 0, errs() << "Started loop"; i < categories.getNumberOfRuns(); i++)
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
			  //for()
		  }*/

		  ShapiroHorwitz::ShapiroHorwitz myShapiro(Pointers, 1, F, 2);
		  myShapiro.printPointsToSet();
	}

	virtual bool runOnFunction(Function &F)
	{
		++FreeCounter;
		errs() << "Free Placement:" << F.getName() << "\n";
		std::vector<ReturnInst*> Returns;
		std::vector<CallInst*> Frees;

		int StackCounter = 1, HeapCounter = 1;

		//Graph pointsToGraph(&categoryValueMap[0]);
		getPointersCategorized(F, 4);

		// Funtion iterator returns an iterator which iterates through the basic block
		/*for(Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)
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
					pointsToGraph.loadConnect(o2, o1);
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
						errs() << "Operand1:" << o1 << ":" << *AI->getOperand(0)->getType();
						errs() << "\tOperand2:" << o2 << ":" << *AI->getOperand(1)->getType();
						errs() << "\n\n";
						pointsToGraph.storeConnect(o2, o1);
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
					pointsToGraph.createVertices(o1, stackLoc);
					errs() << "Alloca: " << o1 << "\n";

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


					if(!pointsToGraph.cloneVertex(o1, o2))
						errs() << "BitCast: Clonning problem:\t" << o2 << "\t" << o1 << "\n";
					else
					{
//						Unionize<std::string> myUnion(pointsToGraph.getVertexAtLabel(o1),
//								pointsToGraph.getVertexAtLabel(o2), categoriesLastRun);
						errs() << "BitCast: Clonning happened:\t" << o2 << "\t" << o1 << "\n";
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
						errs() << "LHS: " << o1 << "\n";
						char* heapLoc = new char[50];
						sprintf(heapLoc, "Malloc%d", HeapCounter++);
						pointsToGraph.createVertices(o1, heapLoc);
					}
				}
			}
		}

		pointsToGraph.createDotFile("pointsToGraph.dot");*/

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
