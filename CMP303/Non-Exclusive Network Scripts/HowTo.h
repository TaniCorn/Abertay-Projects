#pragma once
#ifndef HOWTO_H
#define HOWTO_H


#include "framework\BaseLevel.h"

#include "UIManager.h"
#include "CollisionManager.h"
#include "CollisionDebugBox.h"

#include "Background.h"
class HowTo :
    public BaseLevel
{
public:
	HowTo(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~HowTo();

	void init() override;
	void handleInput(float dt) override;
	void update(float dt);
	void render();
private:
	Cursor* cursor;
	UIManager UI;	int uiResponse;
	Background background;
	sf::Texture* howToImage;
	CollisionManager collisionManager;
	CollisionDebugBox debug;
};

#endif // !HOWTO_H
