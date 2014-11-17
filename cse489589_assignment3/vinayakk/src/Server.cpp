/*
 * Server.cpp
 *
 *  Created on: Nov 14, 2014
 *      Author: vinayak
 */

#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

Server::Server(int id, map<int, ServerDetails> network)
{
	this->id = id;
	this->network = network;
	map<int, ServerDetails>::iterator i = network.find(id);
	port = i->second.get_port();
}

void Server::receive_data(int port)
{
}

void Server::command_execute(string cmd)
{
	if (cmd == "")
		return;
	istringstream iss(cmd);
	vector<string> cmd_list;
	for (string str; iss >> str;)
		cmd_list.push_back(str);
	transform(cmd_list[0].begin(), cmd_list[0].end(), cmd_list[0].begin(),
			::tolower);
	print(cmd + ":" + command_map(cmd_list) + "\n");
	print("> ");
}

string Server::command_map(vector<string> cmd_list)
{
	string cmd = cmd_list[0];
	if (cmd == "academic_integrity")
	{
		print("\“I have read and understood the course academic integrity "
				"policy located at http://www.cse.buffalo.edu/faculty/dimitrio"
				"/courses/cse4589_f14/index.html#integrity\”");
		return "SUCCESS";
	}
	if (cmd == "update" && cmd_list.size() == 4)
	{
		int id1 = atoi(cmd_list[1].c_str());
		int id2 = atoi(cmd_list[2].c_str());
		int cost =
				cmd_list[3] == "inf" ? UINT16_MAX : atoi(cmd_list[3].c_str());
		cost_table.update_cost(id1, id2, cost);
		//TODO generate packet
		//TODO send data to specified servers
		return "SUCCESS";
	}
	if (cmd == "step")
	{
		//TODO generate packet
		//TODO send routing update to neighbors
		return "SUCCESS";
	}
	if (cmd == "packets")
	{
		print(updates_received + "");
		updates_received = 0;
		return "SUCCESS";
	}
	if (cmd == "display")
	{
		return "SUCCESS";
	}
	if (cmd == "disable" && cmd_list.size() == 2)
	{
		return "SUCCESS";
	}
	if (cmd == "crash")
	{
		while(true){}
		return "";
	}
	if (cmd == "dump")
	{
		void* pkt = &generate_packet();
		int size = sizeof(*pkt);
		cse4589_dump_packet(pkt, size);
		return "SUCCESS";
	}
	return "Invalid command or arguments";
}

void Server::start()
{
	int status, sockfd;
	addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_socktype = AI_PASSIVE;

	if ((status = getaddrinfo(ip.c_str(), string(port).c_str(), &hints, &res))
			!= 0)
	{
		print("Error getting address information: " + gai_strerror(status));
		exit(0);
	}
	for (p = res; p != NULL; p = p->ai_next)
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1)
		{
			perror("socket");
			continue;
		}
		status = bind(sockfd, p->ai_addr, p->ai_addrlen);
		if (status < 0)
		{
			perror("bind");
			close(sockfd);
			continue;
		}
		break;
	}
//TODO check the max no. of connections
	listen(sockfd, 2);
	freeaddrinfo(res);
	fd_set read_fds;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	int listener = sockfd;
	fdmax = sockfd;
	FD_SET(sockfd, &master);
	FD_SET(0, &master); //file descriptor for std. IO
	print("> ");
	while (true)
	{
		read_fds = master;
		fflush(stdout);
		//TODO check timeout for select
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(0);
		}

		for (int i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{
				if (i == listener)
					receive_data(i);
				else if (i == 0)
				{
					string cmd;
					getline(cin, cmd);
					command_execute(cmd);
				}
				else
				{
					receive_data(i);
				}
			}
		}
	}
}

void Server::print(string line)
{
	cout << line << endl;
}
