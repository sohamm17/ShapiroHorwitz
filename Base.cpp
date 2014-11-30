#include "Base.h"
#include <iostream>
#include <math.h>
#include <vector>

void Base::setNewNumber(int n)
{
	decimal = n;
	number.clear();
	do
	{
		number.push_back(n % myBase);
		n /= myBase;
	}
	while(n != 0 && myBase != 1);
}

Base::Base():myBase(defaultBase), decimal(0)
{
	number.push_back(0);
}

Base::Base(int base):myBase(base), decimal(0)
{
	number.push_back(0);
}

Base::Base(int base, int initializer): myBase(base)
{
	//int i = 0;
	setNewNumber(initializer);
}

Base::Base(const Base &copy)
{
	this->operator =(copy);
}

Base& Base::operator=(const Base &copy)
{
	myBase = copy.myBase;
	this->number = copy.number;
	return *this;
}

Base& Base::operator=(int number)
{
	setNewNumber(number);
	//this->number = number;
	return *this;
}

//Returning reference because this can be the actual object which is calling overloaded ++
Base& Base::operator++()//prefix increment
{
	//setNewNumber(++decimal);
	int i = 0;
	while(i < (int) number.size())
	{
		int currentNumber = number[i];
		currentNumber += 1;
		if(currentNumber % myBase == 0)
		{
			currentNumber = 0;
			number[i++] = currentNumber;
			if(i == (int) number.size())
				number.push_back(0);//if there's no more position left in vector push 0 in front
		}
		else
		{
			number[i] = currentNumber;
			break;
		}
	}
	return *this;
}

//not returning reference because this should be a new object
Base Base::operator++(int n)
{
	Base tmp = *this;
	if(n == 0)//using for the sake of it
	{
		operator ++();
		return tmp;
	}
	else
		return *(new Base());
}

//Returning reference because this can be the actual object which is calling overloaded ++
Base& Base::operator--()//prefix increment
{
	//setNewNumber(++decimal);
	int i = 0;
	while(i < (int) number.size())
	{
		int currentNumber = number[i];
		currentNumber -= 1;
		if(currentNumber >= 0)
		{
			number[i] = currentNumber;
			if(i == (int)number.size() - 1 && currentNumber == 0 && i != 0)
				number.pop_back();//we don't need leading zeroes
								//i != 0; if we have only one digit then don't pop that back
			break;
		}
		else if(currentNumber < 0)
		{
			currentNumber = myBase - 1;
			number[i++] = currentNumber;
		}
	}
	return *this;
}

//not returning reference because this should be a new object
Base Base::operator--(int n)
{
	Base tmp = *this;
	if(n == 0)//using for the sake of it
	{
		operator --();
		return tmp;
	}
	else
		return *(new Base());
}

int Base::getNumberAtIndex(int n)
{
	//TODO number representation should change
	if(n >= 0 && n < (int)number.size())
	{
		return this->number.at(n);
	}
	else
	{
		return 0;
	}
}

std::vector<int> Base::getFullNumber()
{
	return this->number;
}

int Base::getBase()
{
	return this->myBase;
}

void Base::printNumber()
{
	unsigned int i = (unsigned int) (number.size() - 1);
	do
	{
		std::cout << number[i] << " ";
		if(i == 0)
			break;
		else
			i--;
	}
	while(1);
	std::cout << std::endl;
}

int Base::decimalNumber()
{
	return this->decimal;
}

Base** Base::generateArray(int start, int end, int base)
{
	Base **arr = new Base*[end-start];
	int i, j = 0;
	for(i = start - 1; i < end; i++, j++)
	{
		arr[j] = new Base(base, i);
	}
	return arr;
}

int Base::defaultBase = 10;
int Base::defaultNumber = 0;

/*int main()
{
	int category = 4;
	int numVariables = 20;

	Base** arr = Base::generateArray(1, numVariables, category);


	int i;
	for(i = 0; i < numVariables; i++)
	{
		arr[i]->printNumber();
	}

	vector< vector<int> > categoryBucket(category);

	int numberRuns = 0;
//	Mathematical way to calculate number of runs
//	float logkN = (float)(log10(numVariables) / log10(category));
//
//	if(logkN > (int)logkN)
//		numberRuns = (int) (logkN) + 1;
//	else
//		numberRuns = (int) logkN;

	// tricky way to find the number of runs
	numberRuns = (int) arr[numVariables - 1]->getFullNumber().size();
	for(i = 0; i < numberRuns; i++)
	{
		int j;
		cout << "\nIteration :" << i + 1 << endl;
		for(j = 0; j < category; j++)
		{
			int k;
			categoryBucket[j].clear();
			for(k = 0; k < numVariables; k++)
			{
				if(arr[k]->getNumberAtIndex(i) == j)
					categoryBucket[j].push_back(k);
			}
			cout << j << ":";
			printArr(categoryBucket[j]);
		}
	}
	return 0;
}
*/
