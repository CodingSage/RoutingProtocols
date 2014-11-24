/*
 * DistanceVector.cpp
 *
 *  Created on: Nov 16, 2014
 *      Author: vinayak
 */

#include "DistanceVector.h"

DistanceVector::DistanceVector()
{
}

DistanceVector::~DistanceVector()
{
}

//add or update for a particular server id the cost and next hop server id
void DistanceVector::add_cost(int id, int cost, int host_id)
{
	map<int, int>::iterator i = cost_map.find(id);
	if (i != cost_map.end())
	{
		i->second = cost;
		next_hop.find(id)->second = host_id;
		return;
	}
	pair<int, int> cost_pair(id, cost);
	pair<int, int> host(id, host_id);
	cost_map.insert(cost_pair);
	next_hop.insert(host);
}

void DistanceVector::update_cost(int id, int cost)
{
	map<int, int>::iterator i = cost_map.find(id);
	if (i == cost_map.end())
		return;
	i->second = cost;
}

int DistanceVector::get_cost(int id)
{
	map<int, int>::iterator i = cost_map.find(id);
	if (i == cost_map.end())
		return INFINITE_COST;
	return i->second;
}

int DistanceVector::get_hop_id(int id)
{
	return next_hop.find(id)->second;
}

vector<int> DistanceVector::get_all_hosts()
{
	vector<int> hosts;
	for (map<int, int>::iterator i = cost_map.begin(); i != cost_map.end(); i++)
		hosts.push_back(i->first);
	return hosts;
}
