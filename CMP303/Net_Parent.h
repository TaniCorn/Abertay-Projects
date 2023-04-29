#pragma once
#ifndef NET_PARENT_H
#define NET_PARENT_H

#include <SFML/Network.hpp>
#include "States.h"
#define HOST 1
#define CLIENT 0

struct Net_UserInformation {
	Net_UserInformation() { socket = nullptr; ipAddress = "127.0.0.1"; port = 0; username = "NotDefined"; state = DummyState; prevState = DummyState; };
	sf::TcpSocket* socket;//Only relevant to host
	std::string ipAddress;
	//For selfData this is local port, for other data this is connected port
	UINT16 port;
	std::string username;
	int identifier = 0;
	Net_UserState state;
	static Net_UserState DummyState;
	Net_UserState prevState;
};

sf::Packet& operator <<(sf::Packet& packet, const Net_UserInformation& m);
sf::Packet& operator >>(sf::Packet& packet, Net_UserInformation& m);



class Net_Parent
{
public:
protected:
	static sf::IpAddress ip;

};

#endif // !NET_PARENT_H
