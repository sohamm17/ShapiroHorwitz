/*
 * Edge.cpp
 *
 *  Created on: Nov 25, 2014
 *      Author: sohamgrad
 */


#include "Edge.h"
/////////////////    Edge class    ///////////////////////


Edge::Edge(Vertex * target)
{
	this->target = target;
	//this->type = type;

}

Edge::Edge(Vertex * source, Vertex * target)
{
	this->source = source;
	this->target = target;
}

//Edge::EdgeType Edge::getType()
//{
//	return type;
//}

Vertex* Edge::getTarget()
{
	return target;
}

Vertex* Edge::getSource()
{
	return source;
}
