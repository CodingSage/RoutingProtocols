/**
 * @vinayakk_assignment3
 * @author  Vinayak Karuppasamy <vinayakk@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <map>

#include "../include/global.h"
#include "../include/logger.h"
#include "../include/Server.h"

using namespace std;

void parse_topology(string path, map<int, ServerDetails>* network,
		int* current_server)
{
	ifstream file;
	file.open(path.c_str());
	int no_servers, no_edges;
	file >> no_servers;
	file >> no_edges;
	for (int i = 0; i < no_servers; ++i)
	{
		int sid, sport;
		string sip;
		file >> sid >> sip >> sport;
		ServerDetails server(sip, sport);
		pair<int, ServerDetails> info(sid, server);
		network->insert(info);
	}

	for (int i = 0; i < no_edges; ++i)
	{
		int id1, id2, cost;
		*current_server = id1;
		file >> id1 >> id2 >> cost;
		map<int, ServerDetails>::iterator server = network->find(id1);
		server->second.add_neighbour(id2, cost);
	}
}

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log();

	/* Clear LOGFILE and DUMPFILE */
	fclose(fopen(LOGFILE, "w"));
	fclose(fopen(DUMPFILE, "wb"));

	/*Start Here*/
	//TODO validations
	string topology_path = "";
	long int update_time = 0;
	for (int i = 1; i < argc; ++i)
	{
		string arg(argv[i]);
		if (arg == "-t" && i != argc - 1)
			topology_path = argv[i + 1];
		if (arg == "-i" && i != argc - 1)
			update_time = atoi(argv[i + 1]);
	}
	if (topology_path == "" || update_time == 0)
	{
		cse4589_print_and_log((char*) "Invalid arguments");
		exit(0);
	}
	map<int, ServerDetails> network;
	int current_server;
	parse_topology(topology_path, &network, &current_server);
	Server server(current_server, network);
	server.start();
	return 0;
}
