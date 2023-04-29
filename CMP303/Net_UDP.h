#pragma once
#ifndef NET_UDP_H
#define NET_UDP_H

#include "Net_Parent.h"

class Net_UDP : public Net_Parent
{
public:
	Net_UDP();
	~Net_UDP();

	void RecievingClients();
	bool ConnectToHost(std::string ip, unsigned int port);
	void SetHost(bool host) { hosting = host; }
	static Net_UDP* GetInstance() { return instance; };
private:
	static Net_UDP* instance;
	sf::Socket::Status sockStatus;//Error Codes

	bool hosting;
	sf::IpAddress ip;
	sf::UdpSocket socket;

	std::string text = "Connect to: ";
	std::size_t recieved;
	char buffer[200];
	bool sending;

	std::map<unsigned short, sf::IpAddress> computerID;

	

};

#endif // !NET_UDP_H
