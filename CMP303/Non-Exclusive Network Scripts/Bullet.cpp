#include "Bullet.h"

Bullet::Bullet(float bulVel, float dmg, sf::RenderWindow* win)
{
	bulletVelocity = bulVel; damage = dmg; window = win;
}

void Bullet::InitialDirection(sf::Vector2f b) {
	setVelocity(b.x, b.y);
}
void Bullet::update(float dt) {
	move(velocity * bulletVelocity * dt);
	deathCheck();
}
/// <summary>
/// If the bullet is outside of the viewport, make them dead. Put in here instead of weapon.cpp to be compatible with the tempBullet function.
/// </summary>
void Bullet::deathCheck() {
	setAlive(!(
		(getPosition().x >= (window->getView().getCenter().x + (window->getView().getSize().x / 2.0f) + 300)) ||
		(getPosition().x <= (window->getView().getCenter().x - (window->getView().getSize().x / 2.0f) - 300)) ||
		(getPosition().y >= (window->getView().getCenter().y + (window->getView().getSize().y / 2.0f) + 300)) ||
		(getPosition().y <= (window->getView().getCenter().y - (window->getView().getSize().y / 2.0f) - 300))
		));
}

void Bullet::collisionResponse(GameObject* col) {
	setAlive(false);
}
void Bullet::collisionResponse() {
	setAlive(false);
}