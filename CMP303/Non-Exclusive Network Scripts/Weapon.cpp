#include "Weapon.h"

Weapon::Weapon()
{
	damage = 0; maxBulletsInMag = 0;	bulletTexture.loadFromFile("gfx/Player/Bullet3.png"); isShooting = false;
	if (!font.loadFromFile("font/arial.ttf"))
	{
	}
	magazine.setFont(font);
	magazine.setCharacterSize(24);
	magazine.setString("0/0");
	magazine.setFillColor(sf::Color::White);

	reload.setFont(font);
	reload.setCharacterSize(20);
	reload.setString("Press \"R\" to reload");
	reload.setFillColor(sf::Color(139, 0, 0, 150));
	weaponImage = new sf::Texture();
}

Weapon::~Weapon()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		delete bullets[i];
	}
}

/// <summary>
/// For all alive bullets, update them and then call deathCheck.
/// </summary>
void Weapon::update(float dt)
{
	//For all bullets, if alive, update and deathcheck
	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i]->isAlive()) {
			bullets[i]->update(dt);
			//deathCheck(i);
		}
	}

	//If bulletsInMag is below a certain value, bring up prompt to reload
	reload.setFillColor(sf::Color(0, 0, 0, 0));
	if (bulletsInMag < (maxBulletsInMag * 0.2) + 2)
	{
		reload.setFillColor(sf::Color(139, 0, 0, 150));
		//String states depending on bulletsLeft and bulletsInMag
		if (bulletsLeft == 0 && bulletsInMag == 0) {
			reload.setString("No Ammo");
		}
		else if (bulletsLeft == 0)
		{
			reload.setString("Low Ammo");
		}
		else {
			reload.setString("Press \"R\" to reload");
		}

	}
	//Magazine text, showing bulletsInMag and bulletsLeft
	magazine.setString("Ammo : " + std::to_string(bulletsInMag) + " / " + std::to_string(bulletsLeft));

	RepositionUI();
}
/// <summary>
/// Spawns a bullet with a direction and position.
/// </summary>
void Weapon::spawn(sf::Vector2f spawnPos, sf::Vector2f playerLookDirection) {

	//For all bullets, if dead, spawn one with position and direction.
	for (int i = 0; i < bullets.size(); i++)
	{
		if (!bullets[i]->isAlive()) {
			bullets[i]->setAlive(true);
			bullets[i]->setRotation(VectorHelper::angleBetweenTwoVectors(upDirection, playerLookDirection) + Down);
			bullets[i]->InitialDirection(playerLookDirection);
			bullets[i]->setPosition(spawnPos + (playerLookDirection * 60.f));
			aud->playSoundbyName("Shooting");
			bulletsInMag--;
			return;

		}
	}

}
/// <summary>
/// If the bullet in bullets[i] is outside of the viewport, make them dead.
/// </summary>
void Weapon::deathCheck(int i) {
			
			/*if (bullets[i]->getPosition().x >= (window->getView().getCenter().x + (window->getView().getSize().x / 2.0f)))
			{
				bullets[i]->setAlive(false);
			}
			else if (bullets[i]->getPosition().x <= (window->getView().getCenter().x - (window->getView().getSize().x / 2.0f))) {
				bullets[i]->setAlive(false);
			}
			if (bullets[i]->getPosition().y >= (window->getView().getCenter().y + (window->getView().getSize().y / 2.0f)))
			{
				bullets[i]->setAlive(false);
			}
			else if (bullets[i]->getPosition().y <= (window->getView().getCenter().y - (window->getView().getSize().y / 2.0f))) {
				bullets[i]->setAlive(false);
			}*/
			//If the bullet is outside any of the borders, it will setAlive to false. Branchless programming seems so cool
			bullets[i]->setAlive(!(
				(bullets[i]->getPosition().x >= (window->getView().getCenter().x + (window->getView().getSize().x / 2.0f) + 2000)) || 
				(bullets[i]->getPosition().x <= (window->getView().getCenter().x - (window->getView().getSize().x / 2.0f) - 2000)) || 
				(bullets[i]->getPosition().y >= (window->getView().getCenter().y + (window->getView().getSize().y / 2.0f) + 2000)) || 
				(bullets[i]->getPosition().y <= (window->getView().getCenter().y - (window->getView().getSize().y / 2.0f) - 2000)) 
								));
}
/// <summary>
/// Draws all alive bullets on screen. This will have to be called from player->equippedWeapon.render()
/// </summary>
void Weapon::render() {
	//For all bullets, if alive, draw them on window.
	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i]->isAlive()) {
			window->draw(*bullets[i]);
		}
	}

}

void Weapon::handleInput(float dt, sf::Vector2f bulletSpawnPos, sf::Vector2f bulletSpawnVel)
{
	if (bulletsInMag < maxBulletsInMag && input->isPressed(sf::Keyboard::R) && bulletsLeft > 0)
	{
		Reload();
	}
}
void Weapon::Reload() {

		int bulletsNTR = maxBulletsInMag - bulletsInMag;
		if (bulletsLeft <= bulletsNTR)
		{
			bulletsNTR = bulletsLeft;
		}
		bulletsInMag += bulletsNTR;
		bulletsLeft -= bulletsNTR;
}

void Weapon::RepositionUI()
{
	magazine.setPosition(sf::Vector2f(window->getView().getCenter().x - (window->getView().getSize().x / 2.f) + 50.f,
		window->getView().getCenter().y + (window->getView().getSize().y / 2.f) - 50.f));
		reload.setPosition(sf::Vector2f(window->getView().getCenter()));

}

void Weapon::SetWeaponImage(sf::Texture* image)
{
	weaponImage = image;
}

void Weapon::SetupOneBullet(Bullet* bul, sf::Texture* texture)
{
	bul->setSize(sf::Vector2f(5, rand() % 75 + 35));
	bul->setTexture(texture);
	bul->setAlive(false);
	bul->setCollisionBox(0, 0, 3, 5);
	bul->setOrigin(sf::Vector2f(bul->getCollisionBox().left + bul->getCollisionBox().width, bul->getCollisionBox().top + bul->getCollisionBox().height) / 2.f);
}
