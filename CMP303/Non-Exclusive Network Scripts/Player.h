#ifndef PLAYER_H
#define PLAYER_H


#include "M1911.h"
#include "AK47.h"
#include "Sprites.h"
#include "Framework\VectorHelper.h"
#include <list>
#include "Cursor.h"
#include <array>
#include "Inventory.h"
#include <math.h>

#include "Framework\AudioManager.h"

/// <summary>
/// The player sprite object contains extra functionality and components.
/// <para>The player is still heavily inheritance based however we had to use component base for the weapon. If the majority was component based, we would have to make a render function to render all components.
/// Currently the only render components needed to be referenced here in level and called are:</para>
/// <para> equippedWeapon </para>
/// </summary>
class Player :	public Sprites
{
private:
	void CalculateLookDirection() override;
	void CalculateMovementDirection() override;

	float invincibilityTime = 1.f;//Amount of time player is unable to be hit after taking a hit from any damage sources
	float timeElapsed;//Time elapsed for invincibility

	//TempBullets was a bad idea, if possible sort out at the end and have weapon handle everything, including unequipped weapons. Also discrepency between this and inventory
	#pragma region Weapons
	void UpdateTempBullets(float dt);
	void PushTempBullets();
	void SwitchWeapon(float);
	/// <summary>
	/// <para>When switching weapons, if any bullets remain alive in the original weapon, push those bullet pointers into the tempBullet pointer vector.</para>
	/// We will update and render them until they die, then we will remove them from the tempBullets vector without destroying them. 
	/// It will also be removed when it already exists in the current weapon.
	/// </summary>
	std::vector<Bullet*> tempBullets;
public:
	std::list<Weapon*> weaponsInInventory;//The weapons that the player has found and has in inventory. It should never be empty.
	Weapon* equippedWeapon;//The current weapon the player is using in their hand. Used to shoot. 
	#pragma endregion

	//If we have time, we need to sort out this ui, gameobject discrepency, it's a crap system. Fact, there's no system
	#pragma region UI/Points/Inventory
	float currency;//currency for the player to spend on purchasable things.
	/// <summary>
	/// Adds to currency
	/// </summary>
	/// <param name="points"></param>
	void AddPoints(float points);

	sf::Font font;
	sf::Text points;
	sf::Text hp;
	/// <summary>
	/// Will reposition the UI in player to it's respective position on the screen
	/// </summary>
	void RepositionUI();

	Inventory playerInventory;
	void WeaponShopSelection(Weapon* weaponSelected);
#pragma endregion

public:
	Player(float healthIn, float speedIn, sf::Vector2f sizeIn, float xColliderSize, float yColliderSize, std::vector<AnimationStruct*> anim, Input* inIn, sf::RenderWindow* winIn, sf::Vector2f spawnPosIn, Weapon* availableWeapons);
	Player();
	~Player();

	void update(float) override;
	void handleInput(float) override;
	void render(Player*);


	/// <summary>
	/// When colliding with any objects
	/// </summary>
	void collisionResponse(GameObject* go) override;
	/// <summary>
	/// When hit from another gameobject that can damage the player
	/// </summary>
	void collisionResponse(float damage);

	#pragma region Getters/Setters
	sf::Vector2f getLookDirection() { return lookDirection; }
	std::vector<Bullet*> getTempBullets() { return tempBullets; }
	std::vector<Bullet*> getCurrentWeaponBullets() {return equippedWeapon->bullets;}
	#pragma endregion

	Cursor* cursor;
	int firingType = 0;
	int updateID = 1;
};
#endif // !PLAYER_H
