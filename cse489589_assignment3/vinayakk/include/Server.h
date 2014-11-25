/*
 * Server.h
 *
 *  Created on: Nov 14, 2014
 *      Author: vinayak
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include "SocketLibs.h"
#include "ServerDetails.h"
#include "DistanceVector.h"
#include "Packet.h"
#include "logger.h"

#define INFINITE_COST UINT16_MAX

using namespace std;

class Server
{
	int port;
	int id;
	string ip;
	int timeout;
	map<int, ServerDetails> network;
	int fdmax;
	fd_set master;
	int updates_received;
	void receive_data(int fd);
	void send_data(int server_id);
	void command_execute(string cmd);
	string command_map(vector<string> cmd_list);
	void update_cost(int id1, int id2, int cost);
	void calculate_distance_vector();
	Packet generate_packet();
	void check_updates();
	void send_updates();
	void print(string line);
	string int_to_str(int num);
public:
	Server();
	Server(int id, map<int, ServerDetails> network, int timeout);
	virtual ~Server();
	void start();
};

#endif /* SERVER_H_ */
