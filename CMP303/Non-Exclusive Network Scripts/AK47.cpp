#include "AK47.h"
AK47::AK47(Input* in, sf::RenderWindow* win)
{
	input = in;
	window = win;
	name = "AK47";
	cost = 800;
	damage = 6;
	fireRate = 1 / (fireRate / 60);
	maxBulletsInMag = 30; bulletsInMag = maxBulletsInMag;
	bulletsLeft = 100; maxBullets = 140;

	//bulletTexture.loadFromFile("gfx/beachball.jpg");//If bullet has a different texture
	for (int i = 0; i < 10; i++) {
		bullets.push_back(new Bullet(4000, damage, win));
		Weapon::SetupOneBullet(bullets[i], &bulletTexture);
	}

}

AK47::AK47(sf::RenderWindow* win)
{
	name = "AK47";
	cost = 0;
	damage = 0;
	fireRate = 1 / (fireRate / 60);
	maxBulletsInMag = 30; bulletsInMag = maxBulletsInMag;
	bulletsLeft = 100; maxBullets = 140;

	//bulletTexture.loadFromFile("gfx/beachball.jpg");//If bullet has a different texture
	for (int i = 0; i < 10; i++) {
		bullets.push_back(new Bullet(4000, damage, win));
		Weapon::SetupOneBullet(bullets[i], &bulletTexture);
	}

}
AK47::AK47()
{

}

AK47::~AK47()
{
}

void AK47::handleInput(float dt, sf::Vector2f bulletSpawnPos, sf::Vector2f playerLookDirection)
{
	isShooting = false;
	fr += dt;
	if (input->isLeftMouseDown() && fr > fireRate && bulletsInMag > 0)
	{
		isShooting = true;
		fr = 0;
		spawn(bulletSpawnPos, playerLookDirection);
	}


	Weapon::handleInput(dt, bulletSpawnPos, playerLookDirection);
}
