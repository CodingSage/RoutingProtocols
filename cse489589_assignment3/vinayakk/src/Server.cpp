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
	fdmax = 0;
	this->id = id;
	this->network = network;
	this->timeout = timeout;
	map<int, ServerDetails>::iterator i = this->network.find(id);
	port = i->second.get_port();
	ip = i->second.get_ip();
	this->updates_received = 0;
	for (map<int, ServerDetails>::iterator j = network.begin();
			j != network.end(); j++)
	{
		int vcost = id == j->first ? 0 : INFINITE_COST;
		int vhost = id == j->first ? id : -1;
		i->second.add_if_not_exits(j->first, vcost, vhost);
	}
}

void Server::receive_data(int fd)
{
	char buffer[250];
	sockaddr_storage addr;
	socklen_t len = sizeof(addr);
	int bytecount = recvfrom(fd, &buffer, sizeof(buffer), 0, (sockaddr *) &addr,
			&len);
	if (bytecount == -1)
	{
		perror("recvfrom");
		return;
	}
	updates_received++;
	Pkt pkt;
	memcpy(&pkt, buffer, sizeof(pkt));
	Packet packet(pkt, network);
	int senders_id = 0;
	for (map<int, ServerDetails>::iterator i = network.begin();
			i != network.end(); i++)
	{
		if (i->second.get_ip() == packet.get_sender_ip()
				&& i->second.get_port() == packet.get_sender_port())
		{
			senders_id = i->first;
			i->second.set_update_received(true);
			if(!i->second.is_first_received())
				i->second.set_first_received(true);
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
	cse4589_print_and_log("RECEIVED A MESSAGE FROM SERVER %d\n", senders_id);
	DistanceVector v = packet.get_distance_vector();
	vector<int> h = v.get_all_hosts();
	sort(h.begin(), h.end());
	for(int i = 0; i < h.size(); i++)
		cse4589_print_and_log("%-15d%-15d\n", h[i], v.get_cost(h[i]));
	print("");
	calculate_distance_vector();
}

void Server::calculate_distance_vector()
{
	DistanceVector server = network.find(this->id)->second.get_distance_vector();
	for (map<int, ServerDetails>::iterator i = network.begin();
			i != network.end(); i++)
	{
		int min = INFINITE_COST;
		for (map<int, ServerDetails>::iterator j = network.begin();
				j != network.end(); j++)
		{
			if (j->first == this->id)
				continue;
			int cost = server.get_cost(j->first)
					+ j->second.get_cost(i->first);
			if(cost > INFINITE_COST)
				cost = INFINITE_COST;
			if (cost < min && cost < server.get_cost(i->first))
				server.add_cost(i->first, cost, j->first);
		}
	}
	network.find(this->id)->second.set_distance_vector(server);
	string message = "Updated distance vector for server " + int_to_str(id)
			+ "\n" + server.to_string();
	cse4589_print_and_log((char*) message.c_str());
	print("");
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
	print(cmd + ":" + command_map(cmd_list));
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
		cse4589_print_and_log((char*)int_to_str(updates_received).c_str());
		updates_received = 0;
		return "SUCCESS";
	}
	if (cmd == "display")
	{
		ServerDetails detail = network.find(id)->second;
		vector<int> hosts = detail.get_hosts();
		sort(hosts.begin(), hosts.end());
		for (int i = 0; i < hosts.size(); i++)
		{
			int h_id = hosts[i];
			int h_hop = detail.get_hostid(hosts[i]);
			int h_cost = detail.get_cost(hosts[i]);
			cse4589_print_and_log("%-15d%-15d%-15d\n", h_id, h_hop, h_cost);
		}
		return "SUCCESS";
	}
	if (cmd == "disable" && cmd_list.size() == 2)
	{
		int cmdid = atoi(cmd_list[1].c_str());
		map<int, ServerDetails>::iterator server = network.find(id);
		if(!server->second.is_neighbour())
			return "SUCCESS";
		server->second.set_neighbour(false);
		server->second.add_cost(cmdid, INFINITE_COST);
		return "SUCCESS";
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
	ServerDetails self = network.find(id)->second;
	Packet packet(self.get_ip(), self.get_port(), self.get_distance_vector());
	ServerDetails send_to = network.find(server_id)->second;

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo(send_to.get_ip().c_str(), int_to_str(send_to.get_port()).c_str()
			, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(0);
	}
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1)
		{
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		fprintf(stderr, "talker: failed to bind socket\n");
		exit(0);
	}

	Pkt pkt = packet.get_packet(network);
	int size = sizeof(pkt), b;
	if((b = sendto(sockfd, &pkt, size, 0, p->ai_addr, p->ai_addrlen)) == -1)
	{
		perror("talker: sendto");
		exit(1);
	}
	freeaddrinfo(servinfo);
	close(sockfd);
	string message = "Sent update " + int_to_str(id) + " -> "
			+ int_to_str(server_id);// + "\n"
			//+ self.get_distance_vector().to_string();
	print(message);
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
	ServerDetails self = network.find(id)->second;
	for (map<int, ServerDetails>::iterator i = network.begin();
			i != network.end(); i++)
	{
		if (i->first == this->id || !i->second.is_neighbour())
			continue;
		send_data(i->first);
		if(!i->second.is_first_received())
			continue;
		if (i->second.update_recieved())
		{
			i->second.set_timeout_count(3);
			i->second.set_update_received(false);
		}
		else
		{
			i->second.set_timeout_count(i->second.get_timeout_count() - 1);
			if(i->second.get_timeout_count() == 0)
			{
				self.get_distance_vector().update_cost(i->first, INFINITE_COST);
				i->second.set_neighbour(false);
				calculate_distance_vector();
				print("Server " + int_to_str(i->first) + " timed out");
			}
		}
	}
}

void Server::start()
{
	ServerDetails self = network.find(id)->second;
	struct addrinfo hints, *servinfo, *p;
	int listener;
	int rv;
	int reuse = 1;
	fd_set read_fds;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(self.get_ip().c_str(),
			int_to_str(self.get_port()).c_str(), &hints, &servinfo)) != 0)
	{
		string err(
				"Error getting address information: "
						+ string(gai_strerror(rv)));
		cse4589_print_and_log((char*) err.c_str());
		exit(0);
	}
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1)
		{
			perror("listener: socket");
			continue;
		}
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(listener);
			perror("listener: bind");
			continue;
		}
		break;
		if (p == NULL)
		{
			//TODO log
			fprintf(stderr, "listener: failed to bind socket\n");
			exit(0);
		}
		freeaddrinfo(servinfo);
	}

	FD_SET(0, &master); //IO
	FD_SET(listener, &master);
	fdmax = listener;
	print("");
	fflush(stdout);
	timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	while (true)
	{
		read_fds = master;
		int status = select(fdmax + 1, &read_fds, NULL, NULL, &tv);
		if (status == -1)
		{
			perror("select");
			exit(4);
		}
		if (status == 0)
		{
			tv.tv_sec = timeout;
			tv.tv_usec = 0;
			send_updates();
			continue;
		}
		for (int i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{
				if (i == 0)
				{
					string cmd;
					getline(cin, cmd);
					command_execute(cmd);
				}
				else if (i == listener)
				{
					receive_data(i);
				}
			}
		}
	}
}

void Server::print(string line)
{
	printf(line.c_str());
	printf("\n");
	printf(">");
	fflush(stdout);
}

string Server::int_to_str(int num)
{
	stringstream s;
	s << num;
	return s.str();
}
