#include "Net_Parent.h"
sf::IpAddress Net_Parent::ip = sf::IpAddress::getPublicAddress();

Net_UserState Net_UserInformation::DummyState = Net_UserState(sf::Vector2f(0,0), sf::Vector2f(0,0), 0);
sf::Packet& operator <<(sf::Packet& packet, const Net_UserInformation& m) {
		return packet << m.ipAddress << m.port << m.username;
}
sf::Packet& operator >>(sf::Packet& packet, Net_UserInformation& m)
{
	return packet >> m.ipAddress >> m.port >> m.username;
}

