#include "CollisionManager.h"

CollisionManager::CollisionManager()
{

}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::EnemyCollisions()
{
	for (int i = 0; i < enemyManager->aliveEnemies.size(); i++)
	{
		//Bullet : Enemy Collision
		for (int p = 0; p < player->getCurrentWeaponBullets().size(); p++)
		{
			if (player->getCurrentWeaponBullets()[p]->isAlive())
			{
				if (Collision::checkBoundingBox(enemyManager->aliveEnemies[i], player->getCurrentWeaponBullets()[p])) {
					
					Net_EnemyDamage.push(enemyManager->aliveEnemies[i]->Net_EnemyIdentifier);
					Net_EnemyDamage.push(player->getCurrentWeaponBullets()[p]->getDamage());
						enemyManager->aliveEnemies[i]->collisionResponse(player->getCurrentWeaponBullets()[p]);
						player->getCurrentWeaponBullets()[p]->collisionResponse(NULL);

				}
			}

		}
		//TempBullet : Enemy Collision
		for (int p = 0; p < player->getTempBullets().size(); p++)
		{
			if (Collision::checkBoundingBox(enemyManager->aliveEnemies[i], player->getTempBullets()[p]))
			{
				Net_EnemyDamage.push(enemyManager->aliveEnemies[i]->Net_EnemyIdentifier);
				Net_EnemyDamage.push(player->getCurrentWeaponBullets()[p]->getDamage());
				enemyManager->aliveEnemies[i]->collisionResponse(player->getTempBullets()[p]);
				player->getTempBullets()[p]->collisionResponse(NULL);
			}
		}
		//Player : Enemy Collision
		if (Collision::checkBoundingBox(enemyManager->aliveEnemies[i], player))
		{
			//PlayAnimation, if still in collision by animation end, call player collision.
			player->collisionResponse(enemyManager->aliveEnemies[i]->GetDamage());
		}

	}
}

void CollisionManager::EnemyClientBulletCollision(Net_Player* netPlayer)
{
	for (int i = 0; i < enemyManager->aliveEnemies.size(); i++)
	{
		//Bullet : Enemy Collision
		for (int p = 0; p < netPlayer->equippedWeapon->bullets.size(); p++)
		{
			if (netPlayer->equippedWeapon->bullets[p]->isAlive())
			{
				if (Collision::checkBoundingBox(enemyManager->aliveEnemies[i], netPlayer->equippedWeapon->bullets[p])) {

					netPlayer->equippedWeapon->bullets[p]->collisionResponse();

				}
			}

		}
		//TempBullet : Enemy Collision
		for (int p = 0; p < netPlayer->tempBullets.size(); p++)
		{
			if (Collision::checkBoundingBox(enemyManager->aliveEnemies[i], netPlayer->tempBullets[p]))
			{
				netPlayer->tempBullets[p]->collisionResponse();
			}
		}

	}
}

short CollisionManager::UICollision()
{

	for (int i = 0; i < clickableUIElements.size(); i++)
	{
		if (clickableUIElements[i]->isAlive())
		{
			if (Collision::checkBoundingBox(clickableUIElements[i], sf::Vector2i(cursor->getPosition())))
			{
				//Hover action
				clickableUIElements[i]->collisionResponse(NULL);
				//returns the button in vector clicked. We set up a switch statement in levels to write new functions for the button actions.
				if (clickableUIElements[i]->collisionResponseClick())
				{
					return i;
				}

			}
		}

	}
	return 999;
}

short CollisionManager::InventoryCollision()
{
	for (int i = 0; i < inventoryClicks.size(); i++)
	{
		if (inventoryClicks[i]->isAlive())
		{
			if (Collision::checkBoundingBox(inventoryClicks[i], sf::Vector2i(cursor->getPosition())))
			{		
				//Hover action
				inventoryClicks[i]->collisionResponse(NULL);

				//HoverText
				switch (i)
				{
				case(0):
					//The arrows for player inventories shouldn't exist, YET. They are room for future, if we decide to allow more than 2 weapons in inventory
					break;
				case(1):
					break;
				case(2):
					cursor->SetHoverText("Previous Page");
					break;
				case(3):
					cursor->SetHoverText("Previous Page");
					break;
				default:
					cursor->SetHoverText("");
					break;
				}
				if (i >= 4)
				{
					int p = i - 4;
					cursor->SetHoverText("Buy : " + weaponBox->availableWeapons[p]->GetName());
				}

				//returns the button in vector clicked. We set up a switch statement in levels to write new functions for the button actions.
				if (inventoryClicks[i]->collisionResponseClick())
				{
					return i;
				}

			}
		}

	}
	return 999;
}

void CollisionManager::MapCollisions(float dt)
{
	for (int i = 0; i < mapBounds.size(); i++)
	{

		if (Collision::checkBoundingBox(player, mapBounds[i]))
		{
			player->collisionResponseMap(dt, Collision::getBoundingBoxDirection(player, mapBounds[i]));
		}
	}
	for (int i = 0; i < deathObjects.size(); i++)
	{
		for (int e = 0; e < enemyManager->aliveEnemies.size(); e++)
		{
			if (Collision::checkBoundingBox(enemyManager->aliveEnemies[e], deathObjects[i]))
			{

				enemyManager->aliveEnemies[e]->collisionResponse(deathObjects[i]);
			}
		}
		if (Collision::checkBoundingBox(player, deathObjects[i]))
		{
			player->collisionResponseMap(dt, Collision::getBoundingBoxDirection(player, deathObjects[i]));
		}
	}
	for (int i = 0; i < immovableObjects.size(); i++)
	{
		for (int e = 0; e < enemyManager->aliveEnemies.size(); e++)
		{
			if (Collision::checkBoundingBox(enemyManager->aliveEnemies[e], immovableObjects[i]))
			{

				enemyManager->aliveEnemies[e]->collisionResponseMap(dt, Collision::getBoundingBoxDirection(enemyManager->aliveEnemies[e], immovableObjects[i]));
			}
		}
		if (Collision::checkBoundingBox(player, immovableObjects[i]))
		{
			player->collisionResponseMap(dt, Collision::getBoundingBoxDirection(player, immovableObjects[i]));
		}
	}
	if (Collision::checkBoundingBox(player, weaponBox))
	{
		player->collisionResponse(weaponBox); weaponBox->collisionResponse(player);
	}
}

