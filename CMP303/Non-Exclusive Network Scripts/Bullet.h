#include "framework/GameObject.h"
class Bullet : public GameObject
{
public:
	Bullet(float bulletVelocity, float damage, sf::RenderWindow* window);

	void InitialDirection(sf::Vector2f);
	void update(float) override;
	void collisionResponse(GameObject* collider) override;
	void collisionResponse() ;

	float getDamage() { return damage; }

private:
	void deathCheck();
	float bulletVelocity;
	float damage;
};

