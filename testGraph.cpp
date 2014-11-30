// this file is used to test the GraphClasses methods

#include "GraphClasses.h"
#include <iostream>

int testVertex()
{
	// just to see if the methods run and don't crash or anything
	// won't check for correctness
	Vertex * v1 = new Vertex("v1");
	Vertex * v2 = new Vertex("v2", v1);
	v1->addTarget(v2);
	v1->addTarget("v3");
	Vertex * v4 = new Vertex("v4");
	v4->addTargetsOfOther(v1);
	return 0;

}

int testGraph()
{
	Graph * testG = new Graph(NULL);

	// vertex no edges
	testG->createVertex(std::string("A1"));

	// attach new vertex A2 to A1
	testG->createVertex(std::string("A2"), std::string("A1"));

	// clone A2
	testG->cloneVertex(std::string("A3"), std::string("A2"));



	testG->createVertices("A4", "A5");

	testG->createVertex("A6");

	testG->createEdge("A4", "A6");
/*


	// given two labels correspondng to vertices A and B, this method adds edges from all children of A
	// to all children of B. Called to process "store" calls in LLVM
	testG->storeConnect("A2", "A4");

	testG->createVertex("A7");
	testG->createVertex("A8", "A7");
	testG->createEdge("A2", "A8");

	testG->loadConnect("A2", "A9");

	testG->createDotFile("A.dot");

	*/
	return 0;
}

int testMaps()
{
	std::map<char,int> mymap;
	std::map<char,int>::iterator it;
	mymap['a'] = 5;
	mymap['d'] = 2;
	mymap['a'] = 7;

	for (it=mymap.begin(); it!=mymap.end(); ++it)
	{
		std::cout << it->first << " => " << it->second << '\n';
	}
	return 0;

}

int main()
{
	//testVertex();
	testGraph();
	//testMaps();

	return 0;
}
