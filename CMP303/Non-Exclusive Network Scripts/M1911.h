#pragma once
#ifndef M1911_H
#define M1911_h

#include "Weapon.h"
/// <summary>
/// A Semi-Automatic pistol.
/// </summary>
class M1911 : public Weapon
{
public:
    /// <summary>
    /// Sets up weapon, damage, magazine size. Bullets are constructed here.
    /// </summary>
    M1911(Input*, sf::RenderWindow*);
    M1911(sf::RenderWindow*);//For networked clients
    M1911();
    ~M1911();

    /// <summary>
    /// If left mouse button is pressed, shoot.
    /// </summary>
    void handleInput(float dt, sf::Vector2f spawnPosOfBullet, sf::Vector2f playerLookDirection) override;
};

#endif // !M1911_H