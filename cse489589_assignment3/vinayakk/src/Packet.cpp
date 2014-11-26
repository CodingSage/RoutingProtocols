/*
 * Packet.cpp
 *
 *  Created on: Nov 16, 2014
 *      Author: vinayak
 */

#include "Packet.h"

Packet::Packet()
{
	id = -1;
	this->ip = "";
	this->port = -1;
}

Packet::~Packet()
{
}

Packet::Packet(string ip, int port)
{
	id = -1;
	this->ip = ip;
	this->port = port;
}

Packet::Packet(string ip, int port, DistanceVector vector)
{
	this->id = -1;
	this->ip = ip;
	this->port = port;
	this->vector = vector;
}

Packet::Packet(Pkt packet, map<int, ServerDetails> network)
{
	sockaddr_in addr;
	addr.sin_addr.s_addr = packet.ip;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr.sin_addr), str, INET_ADDRSTRLEN);
	ip = string(str);
	port = ntohs(packet.port);
	for (map<int, ServerDetails>::iterator i = network.begin(); i != network.end(); i++)
	{
		if (i->second.get_ip() == ip)
		{
			id = i->first;
			break;
		}
	}
	DistanceVector v;
	int num_servers = ntohs(packet.fields_updated);
	for (int i = 0; i < num_servers; i++)
	{
		UpdateField field = packet.update_fields[i];
		int sid = ntohs(field.update_id);
		int scost = ntohs(field.cost);
		//For next hop not specified : -1
		v.add_cost(sid, scost, -1);
	}
	this->vector = v;
}

Pkt Packet::get_packet(map<int, ServerDetails> network)
{
	Pkt packet;
	sockaddr_in addr;
	inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr));
	packet.ip = addr.sin_addr.s_addr;
	packet.port = htons(port);
	std::vector<int> hosts = vector.get_all_hosts();
	packet.fields_updated = htons(hosts.size());
	//packet.update_fields = new UpdateField[hosts.size()];
	for (int i = 0; i < hosts.size(); i++)
	{
		ServerDetails details = network.find(hosts[i])->second;
		sockaddr_in add;
		UpdateField field;
		field.cost = htons(vector.get_cost(hosts[i]));
		field.update_id = htons(hosts[i]);
		inet_pton(AF_INET, details.get_ip().c_str(), &(add.sin_addr));
		field.update_ip = add.sin_addr.s_addr;
		field.update_port = htons(details.get_port());
		field.padding = 0;
		packet.update_fields[i] = field;
	}
	return packet;
}

int Packet::get_sender_id()
{
	return id;
}

string Packet::get_sender_ip()
{
	return ip;
}

int Packet::get_sender_port()
{
	return port;
}

void Packet::add_updated_fields(DistanceVector vector)
{
	this->vector = vector;
}

DistanceVector Packet::get_distance_vector()
{
	return vector;
}
