/*
 * Packet.h
 *
 *  Created on: Nov 16, 2014
 *      Author: vinayak
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <map>
#include <vector>
#include <string>

#include "DistanceVector.h"
#include "ServerDetails.h"
#include "SocketLibs.h"
#include "global.h"

using namespace std;

struct UpdateField
{
	uint32_t update_ip;
	uint16_t update_port;
	uint16_t padding;
	uint16_t update_id;
	uint16_t cost;
};

struct Pkt
{
	uint16_t fields_updated;
	uint16_t port;
	uint32_t ip;
	UpdateField update_fields[5];
};

class Packet
{
	int id;
	string ip;
	int port;
	DistanceVector vector;
public:
	Packet();
	Packet(string ip, int port);
	Packet(string ip, int port, DistanceVector vector);
	Packet(Pkt packet, map<int, ServerDetails> network);
	virtual ~Packet();
	Pkt get_packet(map<int, ServerDetails> network);
	int get_sender_id();
	string get_sender_ip();
	int get_sender_port();
	void add_updated_fields(DistanceVector vector);
	DistanceVector get_distance_vector();
};

#endif /* PACKET_H_ */
