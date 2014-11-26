/*
 * ServerDetails.h
 *
 *  Created on: Nov 14, 2014
 *      Author: vinayak
 */

#ifndef SERVERDETAILS_H_
#define SERVERDETAILS_H_

#include <string>
#include <stdint.h>

#include "DistanceVector.h"
#include "logger.h"

class ServerDetails
{
	std::string ip;
	int port;
	DistanceVector cost_table;
	bool neighbour;
	int timeout_count;
	bool update_received;
public:
	ServerDetails();
	ServerDetails(std::string ip, int port);
	ServerDetails(std::string ip, int port, bool is_neighbour);
	virtual ~ServerDetails();
	std::string get_ip();
	int get_port();
	bool is_neighbour();
	void set_neighbour(bool flag);
	void add_neighbour(int id, int cost);
	void add_cost(int id, int cost);
	void add_cost(int id, int cost, int host_id);
	void add_if_not_exits(int id, int cost, int host);
	vector<int> get_hosts();
	int get_cost(int id);
	int get_hostid(int id);
	DistanceVector get_distance_vector();
	void set_distance_vector(DistanceVector vector);
	bool update_recieved();
	void set_update_received(bool flag);
	int get_timeout_count();
	void set_timeout_count(int timeout);
};

#endif /* SERVERDETAILS_H_ */
