/*
 * ServerDetails.cpp
 *
 *  Created on: Nov 14, 2014
 *      Author: vinayak
 */

#include "ServerDetails.h"

ServerDetails::ServerDetails()
{
}

ServerDetails::ServerDetails(std::string ip, int port)
{
	this->ip = ip;
	this->port = port;
}

ServerDetails::~ServerDetails()
{
}

std::string ServerDetails::get_ip()
{
	return ip;
}

int ServerDetails::get_port()
{
	return port;
}
