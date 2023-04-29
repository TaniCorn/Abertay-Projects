#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H

#include "Net_UDP.h"

#include "Net_TCP.h"

#include "framework\BaseLevel.h"

//#include "Cursor.h"
#include "UIManager.h"
#include "CollisionManager.h"
#include "CollisionDebugBox.h"
#include "Background.h"


class MainMenu : public BaseLevel
{
public:
	MainMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~MainMenu();

	void init() override;
	void handleInput(float dt) override;
	void update(float dt);
	void render();
	void clean();
private:

	std::string GetTypedName();
	Cursor* cursor;
	UIManager UI;	int uiResponse;
	Background background;

	CollisionManager collisionManager;
	CollisionDebugBox debug;

	std::string ipTyped = "127.0.0.1";
	std::string tcpPortTyped;
	std::string playerNameTyped;
	Net_TCP* tcpConnection;
};

#endif // !MAINMENU_H