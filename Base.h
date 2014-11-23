#ifndef LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_BASE_H_
#define LLVM_3_4_LIB_TRANSFORMS_FREEPLACEMENT_BASE_H_

#include <iostream>
#include <map>
#include <vector>

class Base
{
	private:
		static int defaultBase;
		static int defaultNumber;
		int myBase;
		std::vector<int> number;
		int decimal;

		void setNewNumber(int n);

	public:
		Base();
		Base(int base);
		Base(int base, int initializer);
		Base(const Base &copy);
		Base& operator=(const Base &copy);

		Base& operator=(int number);

		//Returning reference because this can be the actual object which is calling overloaded ++
		Base& operator++();//prefix increment

		//not returning reference because this should be a new object
		Base operator++(int n);

		//Returning reference because this can be the actual object which is calling overloaded ++
		Base& operator--();//prefix increment

		//not returning reference because this should be a new object
		Base operator--(int n);

		int getNumberAtIndex(int n);

		std::vector<int> getFullNumber();

		int getBase();

		void printNumber();

		int decimalNumber();

		static Base** generateArray(int start, int end, int base);
};

#endif
