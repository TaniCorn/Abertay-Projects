#include "States.h"

sf::Packet& operator <<(sf::Packet& packet, const Net_UserState& m) {
	return packet << m.position.x << m.position.y << m.lookDirection.x << m.lookDirection.y << m.stateNumber;
}
sf::Packet& operator >>(sf::Packet& packet, Net_UserState& m)
{
	return packet >> m.position.x >> m.position.y >> m.lookDirection.x >> m.lookDirection.y >> m.stateNumber;
}