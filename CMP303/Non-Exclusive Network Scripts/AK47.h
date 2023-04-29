#pragma once
#ifndef AK47_H
#define AK47_H

#include "Weapon.h"
/// <summary>
/// A fully automatic AR.
/// </summary>
class AK47 :
    public Weapon
{
public:
    /// <summary>
    /// Sets up weapon, damage, magazine size and firerate. Bullets are constructed here.
    /// </summary>
    AK47(Input*, sf::RenderWindow*);
    AK47(sf::RenderWindow*);
    AK47();
    ~AK47();

    /// <summary>
    /// If left mouse button is held, shoot.
    /// </summary>
    void handleInput(float dt, sf::Vector2f spawnPosOfBullet, sf::Vector2f playerLookDirection) override;
    /// <summary>
    /// firerate is designated as rounds per minute.
    /// </summary>
    float getFirerate() { return fireRate; };
private:
    float fireRate = 600.f; //Set the firerate as rpm
    float fr = 1;//wait time for firerate, uses deltaTime
};

#endif // !AK47_H