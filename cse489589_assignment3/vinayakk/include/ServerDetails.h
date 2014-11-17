/*
 * ServerDetails.h
 *
 *  Created on: Nov 14, 2014
 *      Author: vinayak
 */

#ifndef SERVERDETAILS_H_
#define SERVERDETAILS_H_

#include <string>
#include <stdint.h>

class ServerDetails
{
	std::string ip;
	int port;
public:
	ServerDetails();
	ServerDetails(std::string ip, int port);
	virtual ~ServerDetails();
	std::string get_ip();
	int get_port();
};

#endif /* SERVERDETAILS_H_ */
