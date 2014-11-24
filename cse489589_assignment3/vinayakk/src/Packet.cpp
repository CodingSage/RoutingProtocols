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
	for (int i = 0; i < packet.fields_updated; i++)
	{
		UpdateField field = packet.update_fields[i];
		//For next hop not specified : -1
		v.add_cost(ntohs(field.update_id), ntohs(field.cost), -1);
	}
	this->vector = v;
}

Pkt Packet::get_packet(map<int, ServerDetails> network)
{
	Pkt packet;
	sockaddr_in addr;
	inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr));
	packet.ip = addr.sin_addr.s_addr;
	packet.port = htonl(port);
	std::vector<int> hosts = vector.get_all_hosts();
	packet.fields_updated = htonl(hosts.size());
	packet.update_fields = new UpdateField[hosts.size()];
	for (int i = 0; i < hosts.size(); i++)
	{
		ServerDetails details = network.find(hosts[i])->second;
		sockaddr_in add;
		UpdateField field = packet.update_fields[i];
		field.cost = htonl(vector.get_cost(hosts[i]));
		field.update_id = htonl(hosts[i]);
		inet_pton(AF_INET, details.get_ip().c_str(), &(add.sin_addr));
		field.update_ip = add.sin_addr.s_addr;
		field.update_port = htonl(details.get_port());
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
