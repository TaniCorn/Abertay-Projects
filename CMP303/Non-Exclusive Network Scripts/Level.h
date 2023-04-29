#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"

#include "AnimationStorage.h"

#include "CollisionManager.h"
#include "CollisionDebugBox.h"
#include "TMap.h"

#include "Player.h"
#include "WeaponsBox.h"
#include "Background.h"


#include "UIManager.h"

#include "Net_TCP.h"

#include <thread>
#include "Net_Player.h"

class Level : public BaseLevel {
public:
	Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Level();

	void init() override;
	void clean();
	void handleInput(float dt) override;
	void update(float dt);
	void render();
	
	void Net_SendEnemyDamage();
	void Net_RecieveEnemyDamage();
	void Net_EventTrigger();
	void ChangeClientFire();
	void ReceivingCommands();
	void Net_SendEnemyConsistancy();
	void Net_RecieveEnemyConsistancy();
private:
	///////Textures are best to have a facing position of downwards
	//Back-End Objects
	AnimationStorage animationStorage;
	std::vector<Weapon*> availableWeaponsInLevel;

	//Collision Objects
	CollisionManager collisionManager;
	TMap map;
	std::vector<GameObject*> deathObject;
	std::vector<GameObject*> immovableObject;
	CollisionDebugBox debugCollision;

	//Sprite Objects
	Player* player;
	std::map<int, Net_Player*> clientPlayers;
	std::vector<Sprites*> players;

	EnemyManager enemyManager;
	WeaponsBox* weaponsBox;
	Background background;

	UIManager UI; int uiResponse;

	float exiting;

	Net_TCP* tcpConnection;
	std::thread threads;

	const float networkInterval = 0.1f;
	const float slowNetworkInterval = 1.f;
	float networkTick = 0.0f;
	float slowNetworkTick = 0.0f;
};
#endif // !LEVEL_H