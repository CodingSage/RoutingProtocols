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
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include "ServerDetails.h"
#include "DistanceVector.h"
#include "Packet.h"

using namespace std;

class Server
{
	int port;
	int id;
	string ip;
	map<int, ServerDetails> network;
	DistanceVector cost_table;
	int fdmax;
	fd_set master;
	int updates_received;
	void receive_data(int port);
	void send_data(int port, void* data, int size);
	void command_execute(string cmd);
	string command_map(vector<string> cmd_list);
	Pkt generate_packet();
	void print(string line);
public:
	Server();
	Server(int id, map<int, ServerDetails> network);
	virtual ~Server();
	void start();
};

#endif /* SERVER_H_ */
