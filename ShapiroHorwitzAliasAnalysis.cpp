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

#define DEBUG_TYPE "ShapiroHorwitzAliasAnalysis"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/InitializePasses.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/PassSupport.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
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

STATISTIC(ShapiroHorwitzCounter, "Counts number of pointers");

namespace {
// Hello - The first implementation, without getAnalysisUsage.
struct ShapiroHorwitzAliasAnalysis : public FunctionPass, public AliasAnalysis{
	static char ID; // Pass identification, replacement for typeid
	std::vector< std::map<std::string, int> > categoryValueMap;
	ShapiroHorwitz * myShapiro;
	Function* FuncPoint;
	ShapiroHorwitzAliasAnalysis() : FunctionPass(ID) {
		//initializeShapiroHorwitzAliasAnalysisPass(*PassRegistry::getPassRegistry());
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const
	{
		AliasAnalysis::getAnalysisUsage(AU);
		//AU.addRequired<AliasAnalysis>();
		AU.setPreservesAll();
	}

	static inline bool isInterestingPointer(Value *V) {
	  return V->getType()->isPointerTy()
	      && !isa<ConstantPointerNull>(V);
	}





    virtual void initializePass() {
      InitializeAliasAnalysis(this);
    }

//    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
//    //AliasAnalysis::getAnalysisUsage(AU);
//      AU.addRequired<AliasAnalysis>();
//      //AU.addRequired<TargetLibraryInfo>();
//    }

    virtual ShapiroHorwitzAliasAnalysis::AliasResult alias(const Location &LocA,
                              const Location &LocB) {
     /* assert(AliasCache.empty() && "AliasCache must be cleared after use!");
      assert(notDifferentParent(LocA.Ptr, LocB.Ptr) &&
             "BasicAliasAnalysis doesn't support interprocedural queries.");
      AliasResult Alias = aliasCheck(LocA.Ptr, LocA.Size, LocA.TBAATag,
                                     LocB.Ptr, LocB.Size, LocB.TBAATag);
      // AliasCache rarely has more than 1 or 2 elements, always use
      // shrink_and_clear so it quickly returns to the inline capacity of the
      // SmallDenseMap if it ever grows larger.
      // FIXME: This should really be shrink_to_inline_capacity_and_clear().
      AliasCache.shrink_and_clear();
      return Alias;*/


    	// ----------- Soham - Analysis

    	//errs() << "Starting alias query\n" ;
    	std::string o1, o2;
		{
			raw_string_ostream os1(o1), os2(o2);
			WriteAsOperand(os1, LocA.Ptr, true, FuncPoint->getParent());
			WriteAsOperand(os2, LocB.Ptr, true, FuncPoint->getParent());
		}
    	//errs() << "LocA:" << o1 << "\n";
    	//errs() << "LocB:" << o2 << "\n";
    	//errs() << "MyShapiro:" << myShapiro << "\n";
    	int result = myShapiro->Alias(o1, o2);
    	//errs() << "Result:" << result << "\n";
    	//errs() << "AliasFinished\n";
    	if(result == 0)
    	{
//    		errs() << "No Alias\n";
//    		errs() << "LocA:" << o1 << "\n";
//    		errs() << "LocB:" << o2 << "\n";
    		return AliasAnalysis::NoAlias;
    	}
    	else
    		return AliasAnalysis::alias(LocA, LocB);
    	//return myShapiro->Alias(o1, o2);
    }

    //virtual ModRefResult getModRefInfo(ImmutableCallSite CS, const Location &Loc);

//    virtual ModRefResult getModRefInfo(ImmutableCallSite CS1,
//                                       ImmutableCallSite CS2) {
//      // The AliasAnalysis base class has some smarts, lets use them.
//      return AliasAnalysis::getModRefInfo(CS1, CS2);
//    }

    /// pointsToConstantMemory - Chase pointers until we find a (constant
    /// global) or not.
    //virtual bool pointsToConstantMemory(const Location &Loc, bool OrLocal);

    /// getModRefBehavior - Return the behavior when calling the given
    /// call site.
    //virtual ModRefBehavior getModRefBehavior(ImmutableCallSite CS);

    /// getModRefBehavior - Return the behavior when calling the given function.
    /// For use when the call site is not known.
    //virtual ModRefBehavior getModRefBehavior(const Function *F);

    /// getAdjustedAnalysisPointer - This method is used when a pass implements
    /// an analysis interface through multiple inheritance.  If needed, it
    /// should override this to adjust the this pointer as needed for the
    /// specified pass info.
    virtual void *getAdjustedAnalysisPointer(const void *ID) {
      if (ID == &AliasAnalysis::ID)
        return (AliasAnalysis*)this;
      return this;
    }



//////////////////////////////////////////////////////////////////////////////
///////////////// END OF DECLARATIONS FROM BASICAA       /////////////////////
//////////////////////////////////////////////////////////////////////////////






	void runAnalysis(Function &F, int k)
	{
		  SetVector<Value *> Pointers;
		  SetVector<CallSite> CallSites;
		  SetVector<std::string> PointerString;
		  int StackCounter = 1, HeapCounter = 1;
		  //SetVector<Value *> Loads;
		  //SetVector<Value *> Stores;

		  //Module::GlobalListType globalList = F.getParent()->getGlobalList();
		  for(Module::global_iterator I = F.getParent()->getGlobalList().begin(),
				  E = F.getParent()->getGlobalList().end(); I != E; ++I)
		  {
			  if (I->getType()->isPointerTy())
			  {
					Pointers.insert(I);
					char* stackLoc = new char[50];
					sprintf(stackLoc, "Stack%d", StackCounter++);
					PointerString.insert(stackLoc);
				  //errs() << "Getting Global" << "\n";
			  }
		  }

		  for (Function::arg_iterator I = F.arg_begin(), E = F.arg_end(); I != E; ++I)
		    if (I->getType()->isPointerTy())    // Add all pointer arguments.
		    {
		    	Pointers.insert(I);
				char* stackLoc = new char[50];
				sprintf(stackLoc, "Stack%d", StackCounter++);
				PointerString.insert(stackLoc);
		    }

		  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
		    if (I->getType()->isPointerTy()) // Add all pointer instructions.
		    {
		      Pointers.insert(&*I);
		      if(AllocaInst *AI = dyn_cast<AllocaInst>(&*I))
		      {
		    	  if(AI != NULL)
		    	  {
					  char* stackLoc = new char[50];
					  sprintf(stackLoc, "Stack%d", StackCounter++);
					  PointerString.insert(stackLoc);
		    	  }
		      }
		      //Value* stack = new Value();
		    }
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

				if(CallInst *AI = dyn_cast<CallInst>(&*I))
				{
					if(AI->getCalledFunction()->getName().compare_lower("malloc") == 0)
					{
						char* heapLoc = new char[50];
						sprintf(heapLoc, "Malloc%d", HeapCounter++);
						PointerString.insert(heapLoc);
					}
				}

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
				//errs() << o1 << "\n";
		  }

		  llvm::SetVector<llvm::Value *>::iterator pointerIterate = Pointers.begin();

		  for(; pointerIterate != Pointers.end(); pointerIterate++)
		  {
			  std::string o1;
			  {
				  raw_string_ostream os1(o1);
				  WriteAsOperand(os1, *pointerIterate, true, F.getParent());
			  }
			  PointerString.insert(o1);
		  }
		  myShapiro = new ShapiroHorwitz::ShapiroHorwitz(PointerString, k, F, 2);
		  //myShapiro->printPointsToSet();


		  ///// ------- For printing the alias relations
//			for(Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)
//			{
//				for (BasicBlock::iterator I = BI->begin(), E = BI->end(); I != E; ++I)
//				{
//					for(Function::iterator BI2 = F.begin(), BE2 = F.end(); BI2 != BE2; ++BI2)
//					{
//						for (BasicBlock::iterator I2 = BI2->begin(), E2 = BI2->end(); I2 != E2; ++I2)
//						{
//							std::string o1, o2;
//							  {
//								  raw_string_ostream os1(o1), os2(o2);
//								  WriteAsOperand(os1, I, true, F.getParent());
//								  WriteAsOperand(os2, I2, true, F.getParent());
//							  }
//							//Instruction *AI, *AI2;
//							//if ((AI = dyn_cast<Instruction>(I)) && (AI2 = dyn_cast<Instruction>(I2)))
//							{
//								int alias = myShapiro.Alias(o1, o2);
//								if(alias > 0)
//								{
//									errs() << *I << ", " << *I2 << ":\t";
//									errs() << alias << "\n"; //!= AliasAnalysis::NoAlias)
//								}
//							}
//						}
//					}
//				}
//			}

	}

	virtual bool runOnFunction(Function &F)
	{
		InitializeAliasAnalysis(this);
		FuncPoint = &F;
		++ShapiroHorwitzCounter;
		//errs() << "Function Name:" << F.getName() << "\n";
//		std::vector<ReturnInst*> Returns;
//		std::vector<CallInst*> Frees;

		int i;
		runAnalysis(F, 2);
		myShapiro->printPointsToSetSize();
		return false;
	}
};
}


char ShapiroHorwitzAliasAnalysis::ID = 0;
//INITIALIZE_AG_PASS_BEGIN(ShapiroHorwitzAliasAnalysis, AliasAnalysis, "ShapiroHorwitz",
//                   "Shapiro Horwitz Alias Analysis",
//                   false, true, false)
////INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfo)
//INITIALIZE_AG_PASS_END(ShapiroHorwitzAliasAnalysis, AliasAnalysis, "ShapiroHorwitz",
//                   "Shapiro Horwitz Alias Analysis",
//                   false, true, false)


static RegisterPass<ShapiroHorwitzAliasAnalysis> X("ShapiroHorwitz", "ShapiroHorwitz Alias Analysis.");
static RegisterAnalysisGroup<AliasAnalysis> E(X);
