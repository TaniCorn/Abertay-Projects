#pragma once
#ifndef STATES_H
#define STATES_H

#include <SFML/Network.hpp>

struct Net_UserState {
	Net_UserState() {};
	Net_UserState(sf::Vector2f pos, sf::Vector2f look, UINT8 state) { position = pos; lookDirection= look; stateNumber = state; }
	sf::Vector2f position;
	sf::Vector2f lookDirection;
	UINT8 stateNumber;
};

sf::Packet& operator <<(sf::Packet& packet, const Net_UserState& m);
sf::Packet& operator >>(sf::Packet& packet, Net_UserState& m);

#endif // !STATES_H
