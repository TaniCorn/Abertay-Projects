#ifndef ENEMYMANAGER_H
#define ENEMYMANGER_H



#include "Enemy1.h"
#include "Enemy2.h"
#include <math.h>
#include "Framework\AudioManager.h"
class EnemyManager{
public:
	EnemyManager();
	~EnemyManager();
	void Setup(sf::RenderWindow* win, std::vector<AnimationStruct*>* walker, std::vector<AnimationStruct*>* armoured);



	void update(float dt, std::vector<Sprites*> players);
	void render(sf::RenderWindow* window);

	void spawn();
	void spawn(int identifier, sf::Vector2f position);
	void NextRound(float dt);
	void AutomaticSpawner(float dt);

	void RepositionUI();
	std::vector<Enemy1> zombieWalker;
	std::vector<Enemy2> zombieArmoured;

	std::vector<EnemyBase*> aliveEnemies;

	sf::Text roundCounter;

	AudioManager* aud;
private:
	const int maxEnemiesInLevel = 30;
	int enemiesToSpawn = 0;

	int round = 0;
	float nextRoundWaitTime = 5.f;

	float enemySpawnTime = 1;

	sf::RenderWindow* window;
 sf::Font font;
	sf::Vector2f SpawnPoint(int i);
};

#endif // !ENEMYMANAGER_H