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

/*DistanceVector::DistanceVector(const DistanceVector& vector)
 {
 }*/

DistanceVector::DistanceVector(map<int, int> costs, map<int, int> hosts)
{
	cost_map = costs;
	next_hop = hosts;
}

DistanceVector::~DistanceVector()
{
}

DistanceVector DistanceVector::clone()
{
	map<int, int> new_cost(cost_map);
	map<int, int> new_host(next_hop);
	DistanceVector new_vector(new_cost, new_host);
	return new_vector;
}

void DistanceVector::add_cost(int id, int cost)
{
	add_cost(id, cost, -1);
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

bool DistanceVector::has_host(int id)
{
	map<int, int>::iterator i = cost_map.find(id);
	if (i == cost_map.end())
		return false;
	return true;
}

vector<int> DistanceVector::get_all_hosts()
{
	vector<int> hosts;
	for (map<int, int>::iterator i = cost_map.begin(); i != cost_map.end(); i++)
		hosts.push_back(i->first);
	return hosts;
}

string DistanceVector::to_string()
{
	string str = "";
	stringstream s1, s2;
	for (map<int, int>::iterator i = cost_map.begin(); i != cost_map.end(); i++)
	{
		s1.str("");
		s2.str("");
		s1 << i->second;
		s2 << i->first;
		str += s2.str() + ":" + s1.str() + "\n";
	}
	return str;
}
