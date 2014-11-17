/*
 * Packet.h
 *
 *  Created on: Nov 16, 2014
 *      Author: vinayak
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

struct UpdateField
{
	uint16_t update_id;
	uint16_t cost;
	uint16_t update_port;
	uint32_t update_ip;
};

struct Pkt
{
	uint16_t fields_updated;
	uint16_t port;
	uint32_t ip;
	UpdateField** update_fields;
};

class Packet
{
	Pkt packet;
public:
	Packet();
	virtual ~Packet();
	Pkt get_packet();
};

#endif /* PACKET_H_ */
