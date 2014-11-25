/*
 * DistanceVector.h
 *
 *  Created on: Nov 16, 2014
 *      Author: vinayak
 */

#ifndef DISTANCEVECTOR_H_
#define DISTANCEVECTOR_H_

#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <stdint.h>

#define INFINITE_COST UINT16_MAX

using namespace std;

class DistanceVector
{
	map<int, int> cost_map;
	map<int, int> next_hop;
public:
	DistanceVector();
	virtual ~DistanceVector();
	void update_cost(int id, int cost);
	void add_cost(int id, int cost);
	void add_cost(int id, int cost, int host_id);
	int get_cost(int id);
	int get_hop_id(int id);
	vector<int> get_all_hosts();
	string to_string();
};

#endif /* DISTANCEVECTOR_H_ */
