#ifndef ENEMYBASE_H
#define ENEMYBASE_H


#include "Sprites.h"
#include "Player.h"

class EnemyBase : public Sprites
{
private:

	void CalculateLookDirection() override;

	void CalculateMovementDirection() override;

	void GetPathing();
public:
	static void FindPlayer(Player* player);//Should be depreciated
	void FindTarget(std::vector<Sprites*> players);
	/// <summary>
/// This function should compactly setup all the necessary details for a sprite. It uses the sprite size for the collider size.
/// <para> This should be used for any NPC. This should be called in the constructor. </para>
/// </summary>
/// <param name="health of NPC"></param>
/// <param name="speed of NPC"></param>
/// <param name="size of NPC sprite"></param>
/// <param name="texture sheet for NPC"></param>
	void SetupEntity(float healthIn, float speedIn, sf::Vector2f sizeIn, std::vector<AnimationStruct*> anim);
	/// <summary>
/// This function should compactly setup all the necessary details for a sprite.
/// <para> This should be used for any NPC. This should be called in the constructor. </para>
/// </summary>
/// <param name="health of NPC"></param>
/// <param name="speed of NPC"></param>
/// <param name="size of NPC sprite"></param>
/// <param name="x width of collider size"></param>
/// <param name="y height of collider size"></param>
/// <param name="texture sheet for NPC"></param>
	void SetupEntity(float healthIn, float speedIn, sf::Vector2f sizeIn, float xColliderSize, float yColliderSize, std::vector<AnimationStruct*> anim);


	/// <summary>
	/// This function should play an attack animation. Maybe sound?
	/// </summary>
	void virtual Attack();
	/// <summary>
	/// Contains base functionality for movement, rotation and animation
	/// </summary>
	/// <param name=""></param>
	void virtual update(float) override;


	/// <summary>
	/// Insta Death collision
	/// </summary>
	void collisionResponse(GameObject* collider) override;
	/// <summary>
	/// Collision against bullet
	/// </summary>
	/// <param name="collider"></param>
	void collisionResponse(Bullet* collider);
	void TakeDamage(float damage);

	float GetDamage() { return damage; };
	//Enemies have to have a unique identifier because the damaging works on an alive enemy basis.
	int Net_EnemyIdentifier = 0;
	Sprites* target;
	int targetNumber = 0;
protected:

	float damage = 5;//Damage enemy does to player
	float pointsOffered;// Points given to player when enemy dies

};

#endif // !ENEMYBASE_H