#include "EnemyBase.h"

 static Player* pl;
 void EnemyBase::FindPlayer(Player* player1) {	 pl = player1;}

void EnemyBase::SetupEntity(float healthIn, float speedIn, sf::Vector2f sizeIn, std::vector<AnimationStruct*> anim) {
	Sprites::SetupEntity(healthIn, speedIn, sizeIn, anim[0]->textureForAnimation);
	setAlive(false);
	setOrigin(23, 25);


	spriteAnimations = anim;
	currentAnimation = spriteAnimations[0]->animation;
}

void EnemyBase::SetupEntity(float healthIn, float speedIn, sf::Vector2f sizeIn, float xColliderSize, float yColliderSize, std::vector<AnimationStruct*> anim){
	Sprites::SetupEntity(healthIn, speedIn, sizeIn, xColliderSize, yColliderSize, anim[0]->textureForAnimation);
	setAlive(false);
	setOrigin(23, 25);


	spriteAnimations = anim;
	currentAnimation = spriteAnimations[0]->animation;
}

void EnemyBase::Attack() {
	multiplyMovementSpeed(0.5f);
}

void EnemyBase::update(float dt) {
	//Movement and rotation calculations and updates
	CalculateLookDirection(); CalculateMovementDirection();
	setRotation(VectorHelper::angleBetweenTwoVectors(upDirection, lookDirection) + Down);	move(movementDirection * movementSpeed * dt);
	//Animation
	currentAnimation->animate(dt);	setTextureRect(currentAnimation->getCurrentFrame());

	//Plays "Attack" animation when in range of player, else plays "Walk"
	if (VectorHelper::magnitude(target->getPosition() - getPosition()) < 50.f){	ChooseAnimation("Attack");	}
	else {ChooseAnimation("Walk");	}
}
/// <summary>
/// Look towards player, normalised
/// </summary>
void EnemyBase::CalculateLookDirection() {
	lookDirection = target->getPosition() - getPosition();
	lookDirection = VectorHelper::normalise(lookDirection);
}
/// <summary>
/// Move towards player, normalised
/// </summary>
void EnemyBase::CalculateMovementDirection() {
	movementDirection = target->getPosition() - getPosition() - (lookDirection * 20.f);	//std::cout << std::to_string(movementDirection.x) + " : " + std::to_string(movementDirection.y) << std::endl;
	movementDirection = VectorHelper::normalise(movementDirection);
}

void EnemyBase::collisionResponse(GameObject* collider)
{
	setAlive(false);
	health = maxHealth;
}

void EnemyBase::collisionResponse(Bullet* collider)
{
	float damage = collider->getDamage();
	TakeDamage(damage);
}
void EnemyBase::TakeDamage(float damage) {
	health -= damage;
	if (health < 0)
	{
		pl->AddPoints(pointsOffered);
		setAlive(false);
		health = maxHealth;
	}
}
void EnemyBase::FindTarget(std::vector<Sprites*> players) {
	//VectorHelper::magnitude(pl->getPosition() - getPosition()) < 50.f
	float closest = 9999999;
	int index = -1;
	for (int i = 0; i < players.size(); i++)
	{
		float dist = VectorHelper::magnitude(players[i]->getPosition() - getPosition());
		if (dist < closest)
		{
			closest = dist;
			/*if (players[i]->isAlive())
			{
				index = i;

			}*/
			index = i;

		}
	}
	if (index == -1)
	{
		return;
	}
	target = players[index];
	targetNumber = index;
}
#pragma region Unused

/// <summary>
/// For more complex pathfinding, improvement upon Caclculate, movement and look direction
/// </summary>
void EnemyBase::GetPathing() {
	
}

#pragma endregion