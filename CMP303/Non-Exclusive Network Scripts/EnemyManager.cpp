#include "EnemyManager.h"
EnemyManager::EnemyManager() {

}

EnemyManager::~EnemyManager() {
	//for (int i = 0; i < aliveEnemies.size(); i++)
	//{
	//	delete aliveEnemies[i];
	//}
	aliveEnemies.clear();
	window = nullptr;

}

void EnemyManager::Setup(sf::RenderWindow* win, std::vector<AnimationStruct*>* walker, std::vector<AnimationStruct*>* armoured)
{
	window = win;
	for (int i = 0; i < 20; i++) {
		zombieWalker.push_back(Enemy1(10, 95, sf::Vector2f(55, 108), 55, 55, *walker));
		zombieWalker[i].Net_EnemyIdentifier = i;
	}
	for (int i = 0; i < 20; i++)
	{
		zombieArmoured.push_back(Enemy2(30, 50, sf::Vector2f(55, 108), 55, 55, *armoured));
		zombieArmoured[i].Net_EnemyIdentifier = 20 + i;
	}

	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "FAL";
	}
	roundCounter.setFont(font);
	roundCounter.setCharacterSize(24);
	roundCounter.setString("Round : 0");
	roundCounter.setFillColor(sf::Color::White);
	roundCounter.setOrigin(150, 50);

	enemiesToSpawn = 0;
	round = 0;
}

void EnemyManager::update(float dt, std::vector<Sprites*> players)
{
	//Manages the aliveEnemy vector
	for (int i = 0; i < aliveEnemies.size(); i++) {
		if (aliveEnemies[i]->isAlive())
		{
			aliveEnemies[i]->FindTarget(players);
			aliveEnemies[i]->update(dt);
		}
		else {
			aud->playSoundbyName("ZombieDeath");
			aliveEnemies.erase(aliveEnemies.begin() + i);
		}

	}

	//std::cout << "EnemiesToSpawn : " + std::to_string(enemiesToSpawn) + "   | currentEnemiesInLevel : " + std::to_string(aliveEnemies.size()) << std::endl;

	//While enemiesToSpawn isn't less than 0 : Auto-Spawner. Otherwise if player has eliminated all enemies, next round initiates.
	if (enemiesToSpawn <= 0 && aliveEnemies.empty())
	{
		NextRound(dt);
	}
	if (enemiesToSpawn > 0)
	{
		AutomaticSpawner(dt);
	}

	RepositionUI();
}

void EnemyManager::render(sf::RenderWindow* window) {
	for (int i = 0; i < aliveEnemies.size(); i++) {
		window->draw(*aliveEnemies[i]);
	}
}



/// <summary>
/// Starts next round after a wait time period is over. Does not check any other states.
/// </summary>
/// <param name="dt"></param>
void EnemyManager::NextRound(float dt)
{
	//Show next round will start soon

	//Once wait time is up, round goes up and enemies to spawn gets a new amount
	nextRoundWaitTime -= dt;
	if (nextRoundWaitTime < 0)
	{
		round++;
		enemiesToSpawn = ceil((round * round) - (2 * round) + 10);
		nextRoundWaitTime = 4;
		roundCounter.setString("Round : " + std::to_string(round));
	}


}

/// <summary>
/// Spawns an enemy in a random amount of time. Does not check any other states
/// </summary>
void EnemyManager::AutomaticSpawner(float dt)
{
	//Once enemySpawnTime reaches 0, spawn an enemy, increasingly faster
	enemySpawnTime -= dt;
	if (enemySpawnTime < 0)
	{
		spawn();
		enemySpawnTime = rand() % 4 - log10f(round);
		std::cout << "Rands:" << enemySpawnTime;

		//std::cout << "ESP: " + std::to_string(enemySpawnTime) + ". Log: " + std::to_string(log10f(round)) << std::endl;
	}
}

void EnemyManager::RepositionUI()
{
	roundCounter.setPosition(sf::Vector2f(window->getView().getCenter() + (window->getView().getSize() / 2.f)));
}

/// <summary>
/// Returns a spawnpoint inbetween hard coded parameters
/// </summary>
/// <param name="i"></param>
/// <returns></returns>
sf::Vector2f EnemyManager::SpawnPoint(int i)
{
	switch (i)
	{
	case(0):
		return sf::Vector2f(-10, rand() % 2070);
		break;
	case(1):
		return sf::Vector2f(2070, rand() % 2070);
		break;
	case(2):
		return sf::Vector2f(rand() % 2070, -10);
		break;
	case(3):
		return sf::Vector2f(rand() % 2070, 2070);
		break;
	default:
		return sf::Vector2f(0, 0);
		break;
	}
}

void EnemyManager::spawn() {

	if (aliveEnemies.size() >= maxEnemiesInLevel)
	{
		return;
	}

	//Random Spawn of enemy.
	//Pushes 1 non alive enemies into aliveEnemies.
	#pragma region RandomSpawnEnemy
		int i;
		i = rand() % 2;
		i = i * !(round < 3);
		if (i == 0)
		{
			for (i = 0; i < zombieArmoured.size(); i++)
			{
				if (!zombieArmoured[i].isAlive())
				{
					aliveEnemies.push_back(&zombieArmoured[i]);
					aliveEnemies.back()->setAlive(true);
					aliveEnemies.back()->setPosition(SpawnPoint(rand() % 5));
					
					break;
				}
			}


		}
		else {
			for (i = 0; i < zombieWalker.size(); i++)
			{
				if (!zombieWalker[i].isAlive())
				{
					aliveEnemies.push_back(&zombieWalker[i]);
					aliveEnemies.back()->setAlive(true);
					aliveEnemies.back()->setPosition(SpawnPoint(rand() % 5));
					break;
				}
			}
		}
	#pragma endregion

		enemiesToSpawn--;
}

void EnemyManager::spawn(int identifier, sf::Vector2f position)
{
	for (int i = 0; i < 20; i++)
	{
		if (zombieWalker[i].Net_EnemyIdentifier == identifier)
		{
			aliveEnemies.push_back(&zombieWalker[i]);
			aliveEnemies.back()->setAlive(true);
			aliveEnemies.back()->setPosition(position);
			return;
		}
		if (zombieArmoured[i].Net_EnemyIdentifier == identifier)
		{
			aliveEnemies.push_back(&zombieArmoured[i]);
			aliveEnemies.back()->setAlive(true);
			aliveEnemies.back()->setPosition(position);
			return;
		}

	}

}



