#pragma once
#ifndef LOBBY_H
#define LOBBY_H


#include "Net_TCP.h"


#include "framework\BaseLevel.h"

//#include "Cursor.h"
#include "UIManager.h"
#include "CollisionManager.h"
#include "CollisionDebugBox.h"
#include "Background.h"
#include <thread>

class Lobby : public BaseLevel
{
public:
	Lobby(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Lobby();
	void init() override;
	void handleInput(float dt) override;
	void update(float dt);
	void render();
	void clean();
	void AddPlayer(std::string playername);
	void RemovePlayer();
	void ReceivingCommands();
private:
	Cursor* cursor;
	UIManager UI;	int uiResponse;
	std::vector<UIElement> playersUI; 
	Background background;

	CollisionManager collisionManager;
	CollisionDebugBox debug;

	Net_TCP* tcpConnection;

	std::thread threads;
};

#endif // !LOBBY_H