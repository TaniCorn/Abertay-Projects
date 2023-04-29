#include "Net_UDP.h"
#include <iostream>

Net_UDP* Net_UDP::instance;
Net_UDP::Net_UDP() :
	buffer("NULL"),
	hosting(1),
	sending(1),
	recieved(666),
	sockStatus(sf::Socket::NotReady)
{
	instance = this;

	ip = sf::IpAddress::getLocalAddress();
	ip = sf::IpAddress("127.0.0.1");
	std::cout << "Your IP address is: " << ip << std::endl;

	std::cout << "UDP: Initialising\n";
	//Binding our socket
	if (socket.bind(sf::UdpSocket::AnyPort) != sf::Socket::Done)
	{
		std::cout << "UDP: You failed to bind to a port\n";
		std::cout << "UDP HAS FAILED";
		delete instance;
		return;
	}
	std::cout << "UDP: Your Socket Port is:" << socket.getLocalPort() << "\n";
}

Net_UDP::~Net_UDP()
{
	socket.unbind();
}

/// <summary>
/// Make sure we are not blocking at this point
/// </summary>
void Net_UDP::RecievingClients()
{
	if (hosting != HOST)
		return;
	socket.setBlocking(false);
	sf::IpAddress rIP;
	unsigned short port;
	socket.receive(buffer, sizeof(buffer), recieved, rIP, port);//recieve the port number and ip from  client sending to our port and ip
	if (recieved > 0)
	{
		computerID[port] = rIP;//Add client to computerIDs
		std::cout << "UDP: Added new connection. IP: " << rIP << " . Port: " << port << std::endl;
	}

}

bool Net_UDP::ConnectToHost(std::string ip, unsigned int port)
{
	if (hosting != CLIENT)
		return false;

	
		unsigned int serverPort = port;
		sf::IpAddress sendIP(ip);
		sockStatus = socket.send(text.c_str(), text.length() + 1, sendIP, serverPort);
		if (sockStatus == sf::Socket::Error)
		{
			std::cout << "UDP:Error trying to establish connection\n";
			return false;
		}
		if (sockStatus == sf::Socket::NotReady)
		{
			std::cout << "UDP:Wrong port/Socket is not ready to recieve\n";
			return false;
		}
		if (sockStatus == sf::Socket::Done)
		{
			std::cout << "UDP:Connection successfully established!\n";
			return true;
		}
		return false;
}

//void Net_UDP::CommunicationLoop()
//{
//
//	while (true)
//	{
//		if (hosting == HOST)
//		{
//			std::cout << "\n Send All Clients a message: ";//Connect manually to IP
//			std::getline(std::cin, text);
//			std::map<unsigned short, sf::IpAddress>::iterator tempIterator;
//			for (tempIterator = computerID.begin(); tempIterator != computerID.end(); tempIterator++)
//			{
//				socket.send(text.c_str(), text.length() + 1, tempIterator->second, tempIterator->first);
//			}
//			if (text == "quit")
//			{
//				break;
//			}
//		}
//		else if (hosting == CLIENT) {
//			std::cout << "Waiting to recieve data\n";
//			sf::IpAddress tempIP;
//			unsigned short tempPort;
//			socket.receive(buffer, sizeof(buffer), recieved, tempIP, tempPort);
//			if (recieved > 0)
//			{
//				std::cout << "Recieved: " << buffer << std::endl;
//			}
//			if (buffer == "quit")
//			{
//				break;
//			}
//		}
//	}
//
//}
