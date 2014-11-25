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

Server::Server(int id, map<int, ServerDetails> network, int timeout)
{
	this->id = id;
	this->network = network;
	this->timeout = timeout;
	map<int, ServerDetails>::iterator i = network.find(id);
	port = i->second.get_port();
	ip = i->second.get_ip();
	this->updates_received = 0;
}

void Server::receive_data(int fd)
{
	updates_received++;
	Pkt buffer;
	sockaddr_storage addr;
	socklen_t len = sizeof(addr);
	int bytecount = recvfrom(fd, &buffer, sizeof(buffer), 0, (sockaddr*) &addr,
			&len);
	Packet packet(buffer, network);
	int senders_id = 0;
	for (map<int, ServerDetails>::iterator i = network.begin();
			i != network.end(); i++)
	{
		if (i->second.get_ip() == packet.get_sender_ip())
		{
			senders_id = i->first;
			i->second.set_update_received(true);
			break;
		}
	}
	map<int, ServerDetails>::iterator i = network.find(senders_id);
	if (i == network.end())
	{
		ServerDetails detail(packet.get_sender_ip(), packet.get_sender_port(),
				true);
		detail.set_distance_vector(packet.get_distance_vector());
		pair<int, ServerDetails> new_server(senders_id, detail);
		network.insert(new_server);
	}
	else
	{
		i->second.set_distance_vector(packet.get_distance_vector());
	}
	string message = "Update received by " + int_to_str(this->id)
			+"from server " + int_to_str(senders_id);
	cse4589_print_and_log((char*)message.c_str());
	calculate_distance_vector();
}

void Server::calculate_distance_vector()
{
	DistanceVector server =
			network.find(this->id)->second.get_distance_vector();
	for (map<int, ServerDetails>::iterator i = network.begin();
			i != network.end(); i++)
	{
		if (!i->second.is_neighbour())
			continue;
		int min = INFINITE_COST;
		for (map<int, ServerDetails>::iterator j = network.begin();
				j != network.end(); j++)
		{
			if (j->first == this->id)
				continue;
			int cost = j->second.get_cost(this->id)
					+ j->second.get_cost(i->first);
			if (cost < min)
				server.add_cost(i->first, cost, j->first);
		}
	}
	string message = "Updated distance vector for server " + int_to_str(id)
			+ server.to_string();
	cse4589_print_and_log((char*)message.c_str());
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
		int cost = cmd_list[3] == "inf" ?
				INFINITE_COST : atoi(cmd_list[3].c_str());
		update_cost(id1, id2, cost);
		Packet packet = generate_packet();
		//TODO send data to specified servers
		return "SUCCESS";
	}
	if (cmd == "step")
	{
		for (map<int, ServerDetails>::iterator i = network.begin();
				i != network.end(); i++)
		{
			if (i->second.is_neighbour())
				send_data(i->first);
		}
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
		ServerDetails detail = network.find(id)->second;
		vector<int> hosts = detail.get_hosts();
		//TODO check sort order
		sort(hosts.begin(), hosts.end());
		for (int i = 0; i < hosts.size(); i++)
		{
			printf("%­15d%­15d%­15d\n", hosts[i], detail.get_hostid(hosts[i]),
					detail.get_cost(hosts[i]));
			return "SUCCESS";
		}
	}
	if (cmd == "disable" && cmd_list.size() == 2)
	{
		int cmdid = atoi(cmd_list[1].c_str());
		map<int, ServerDetails>::iterator server = network.find(id);
		for (map<int, ServerDetails>::iterator i = network.begin();
				i != network.end(); i++)
		{
			if (i->second.is_neighbour() && i->first == cmdid)
			{
				//server->second.add_cost(i->first, INFINITE_COST);
				server->second.set_neighbour(false);
				return "SUCCESS";
			}
		}
		return "";
	}
	if (cmd == "crash")
	{
		while (true)
		{
		}
		return "";
	}
	if (cmd == "dump")
	{
		Pkt pkt = generate_packet().get_packet(network);
		int size = sizeof(pkt);
		cse4589_dump_packet(&pkt, size);
		return "SUCCESS";
	}
	return "Invalid command or arguments";
}

void Server::send_data(int server_id)
{
	ServerDetails details = network.find(id)->second;
	Packet packet(details.get_ip(), details.get_port(),
			details.get_distance_vector());
	ServerDetails server_details = network.find(server_id)->second;

	addrinfo hint, *servinfo, *p;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_DGRAM;
	string port = int_to_str(server_details.get_port());
	//TODO maybe error handling
	getaddrinfo(server_details.get_ip().c_str(), port.c_str(), &hint, &servinfo);
	int sockfd;
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1)
		{
			perror("socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("connect");
			continue;
		}
		break;
	}
	Pkt pkt = packet.get_packet(network);
	sendto(sockfd, &pkt, sizeof(pkt), 0, (sockaddr*) servinfo,
			sizeof(*servinfo));
	freeaddrinfo(servinfo);
	close(sockfd);
	string message = "Sending update to " + int_to_str(server_id)
			+ " from server " + int_to_str(id)
			+ details.get_distance_vector().to_string();
	cse4589_print_and_log((char*)message.c_str());
}

void Server::update_cost(int id1, int id2, int cost)
{
	DistanceVector vector = network.find(id1)->second.get_distance_vector();
	vector.update_cost(id2, cost);
}

Packet Server::generate_packet()
{
	ServerDetails server = network.find(id)->second;
	Packet packet(server.get_ip(), server.get_port(),
			server.get_distance_vector());
	return packet;
}

void Server::send_updates()
{
	for (map<int, ServerDetails>::iterator i = network.begin();
			i != network.end(); i++)
	{
		if (i->first != this->id)
			continue;
		if (i->second.update_recieved())
		{
			i->second.set_timeout_count(3);
			i->second.set_update_received(false);
		}
		else
			i->second.set_timeout_count(i->second.get_timeout_count() - 1);
		if (i->second.is_neighbour())
			send_data(i->first);
	}
}

void Server::start()
{
	int status, sockfd;
	addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_socktype = AI_PASSIVE;

	if ((status = getaddrinfo(ip.c_str(), int_to_str(port).c_str(), &hints, &res)) != 0)
	{
		string err("Error getting address information: " + string(gai_strerror(status)));
		cse4589_print_and_log((char*)err.c_str());
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
		timeval tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		int t = select(fdmax + 1, &read_fds, NULL, NULL, &tv);
		if (t == 0)
			send_updates();
		if (t == -1)
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

string Server::int_to_str(int num)
{
	stringstream s;
	s << num;
	return s.str();
}
