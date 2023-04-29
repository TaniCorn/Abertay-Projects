#include "M1911.h"

M1911::M1911(Input* in, sf::RenderWindow* win)
{
	input = in;
	window = win;
	name = "M1911";
	cost = 250;
	damage = 14;
	maxBulletsInMag = 7; bulletsInMag = maxBulletsInMag;
	bulletsLeft = 20; maxBullets = 30;
	//bulletTexture.loadFromFile("gfx/beachball.jpg");//If bullet has a different texture
	
	for (int i = 0; i < 7; i++) {
		bullets.push_back(new Bullet(2000, damage, win));
		Weapon::SetupOneBullet(bullets[i], &bulletTexture);
		/*bullets[i]->setSize(sf::Vector2f(3, 55));
		bullets[i]->setTexture(&bulletTexture);
		bullets[i]->setAlive(false);
		bullets[i]->setCollisionBox(0, 0, 3, 5);
		bullets[i]->setOrigin(sf::Vector2f(bullets[i]->getCollisionBox().left + bullets[i]->getCollisionBox().width, bullets[i]->getCollisionBox().top + bullets[i]->getCollisionBox().height)/2.f);*/

	}


}
M1911::M1911(sf::RenderWindow* win) {
	window = win;
	name = "M1911";
	cost = 0;
	damage = 0;
	maxBulletsInMag = 7; bulletsInMag = maxBulletsInMag;
	bulletsLeft = 20; maxBullets = 30;
	//bulletTexture.loadFromFile("gfx/beachball.jpg");//If bullet has a different texture

	for (int i = 0; i < 7; i++) {
		bullets.push_back(new Bullet(2000, damage, win));
		Weapon::SetupOneBullet(bullets[i], &bulletTexture);
		/*bullets[i]->setSize(sf::Vector2f(3, 55));
		bullets[i]->setTexture(&bulletTexture);
		bullets[i]->setAlive(false);
		bullets[i]->setCollisionBox(0, 0, 3, 5);
		bullets[i]->setOrigin(sf::Vector2f(bullets[i]->getCollisionBox().left + bullets[i]->getCollisionBox().width, bullets[i]->getCollisionBox().top + bullets[i]->getCollisionBox().height)/2.f);*/

	}
}
M1911::M1911()
{

}

M1911::~M1911()
{
}

void M1911::handleInput(float dt, sf::Vector2f bulletSpawnPos, sf::Vector2f playerLookDirection)
{
	isShooting = false;
	if (input->isLeftMousePressed() && bulletsInMag > 0)
	{
		isShooting = true;
		spawn(bulletSpawnPos, playerLookDirection);
	}

	Weapon::handleInput(dt, bulletSpawnPos, playerLookDirection);
}
