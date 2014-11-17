/*
 * DistanceVector.h
 *
 *  Created on: Nov 16, 2014
 *      Author: vinayak
 */

#ifndef DISTANCEVECTOR_H_
#define DISTANCEVECTOR_H_

#include <map>

using namespace std;

class DistanceVector
{
	map<int, int> cost_map;
	map<int, string> next_hop;
public:
	DistanceVector();
	virtual ~DistanceVector();
	void update_cost(int id1, int id2, int cost);
	void add_cost(int id, int cost, string host);
	int get_cost(int id);
};

#endif /* DISTANCEVECTOR_H_ */
