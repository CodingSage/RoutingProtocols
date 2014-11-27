/*
 * ServerDetails.cpp
 *
 *  Created on: Nov 14, 2014
 *      Author: vinayak
 */

#include "ServerDetails.h"

ServerDetails::ServerDetails()
{
	ip = "";
	port = -1;
	neighbour = false;
	timeout_count = 3;
	update_received = false;
	first_received = false;
}

ServerDetails::ServerDetails(std::string ip, int port)
{
	this->ip = ip;
	this->port = port;
	this->neighbour = false;
	timeout_count = 3;
	update_received = false;
	first_received = false;
}

ServerDetails::ServerDetails(std::string ip, int port, bool is_neighbour)
{
	this->ip = ip;
	this->port = port;
	this->neighbour = is_neighbour;
	timeout_count = 3;
	update_received = false;
	first_received = false;
}

ServerDetails::~ServerDetails()
{
}

std::string ServerDetails::get_ip()
{
	return ip;
}

int ServerDetails::get_port()
{
	return port;
}

bool ServerDetails::is_neighbour()
{
	return neighbour;
}

void ServerDetails::set_neighbour(bool flag)
{
	neighbour = flag;
}

void ServerDetails::add_neighbour(int id, int cost)
{
	cost_table.add_cost(id, cost, id);
}

//function used for DistanceVectors of other servers, since
//the id of the node to which the packet should be passed
//gets assigned 0
void ServerDetails::add_cost(int id, int cost)
{
	cost_table.add_cost(id, cost);
}

//function used for DistanceVectors of the server itself, next node
//to which packet should be passed is captured (host_id)
//to get to host (id) with cost (cost)
void ServerDetails::add_cost(int id, int cost, int host_id)
{
	cost_table.add_cost(id, cost, host_id);
}

void ServerDetails::add_if_not_exits(int id, int cost, int host)
{
	if(!cost_table.has_host(id))
		cost_table.add_cost(id, cost, host);
}

vector<int> ServerDetails::get_hosts()
{
	return cost_table.get_all_hosts();
}

int ServerDetails::get_cost(int id)
{
	return cost_table.get_cost(id);
}

int ServerDetails::get_hostid(int id)
{
	return cost_table.get_hop_id(id);
}

DistanceVector ServerDetails::get_distance_vector()
{
	return cost_table;
}

void ServerDetails::set_distance_vector(DistanceVector vector)
{
	this->cost_table = vector;
}

bool ServerDetails::update_recieved()
{
	return update_received;
}

void ServerDetails::set_update_received(bool flag)
{
	update_received = flag;
}

int ServerDetails::get_timeout_count()
{
	return timeout_count;
}

void ServerDetails::set_timeout_count(int timeout)
{
	if(timeout > 3 || !neighbour)
		return;
	timeout_count = timeout;
	if(timeout_count == 0)
		neighbour = false;
}

bool ServerDetails::is_first_received()
{
	return first_received;
}

void ServerDetails::set_first_received(bool flag)
{
	first_received = flag;
}
