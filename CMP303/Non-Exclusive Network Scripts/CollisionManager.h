#pragma once
#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H



#include "EnemyManager.h"
#include "Framework\Collision.h"
#include "WeaponsBox.h"
#include "Net_Player.h"
#include <queue>
class CollisionManager
{
public:
    CollisionManager();
    ~CollisionManager();
    void EnemyCollisions();
    void EnemyClientBulletCollision(Net_Player* netplayer);
    short UICollision();
    short InventoryCollision();
    void MapCollisions(float dt);

    EnemyManager* enemyManager;
    Player* player;
    Cursor* cursor;
    WeaponsBox* weaponBox;
    std::vector<UIElement*> clickableUIElements;
    std::vector<UIElement*> inventoryClicks;

    std::vector<GameObject*> immovableObjects;
    std::vector<GameObject*> deathObjects;
    std::vector<GameObject*> mapBounds;

    std::queue<int> Net_EnemyDamage;

};

#endif // !COLLISIONMANAGER_H