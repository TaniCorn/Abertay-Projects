#include "Player.h"
#pragma region CONSTRUCTORS_AND_DESTRUCTORS
Player::Player(float healthIn, float speedIn, sf::Vector2f sizeIn, float xColliderSize, float yColliderSize, std::vector<AnimationStruct*> anim, Input* inIn, sf::RenderWindow* winIn, sf::Vector2f spawnPosIn, Weapon* availableWeapons) {
	//Sets up player
	Sprites::SetupEntity(healthIn, speedIn, sizeIn, xColliderSize, yColliderSize, anim[0]->textureForAnimation, inIn, winIn, spawnPosIn);
	setTextureRect(sf::IntRect(0, 0, 300, 512)); 
	setAlive(true);
	setOrigin(23, 25);

	//Cursor and weapons
	#pragma region SETUP_PLAYER_COMPONENTS
		cursor = new Cursor(winIn, inIn);

	weaponsInInventory.push_back(availableWeapons);
	equippedWeapon = weaponsInInventory.front(); //equippedWeapon = weaponsInInventory.back();
	playerInventory.inventorySlots.push_back(new InventorySlots(weaponsInInventory.front()->GetName(), "Ammo: " + std::to_string(weaponsInInventory.front()->GetBulletsLeft()), anim[0]->textureForAnimation, &font));
	playerInventory.Setup("Inventory", inIn);
	#pragma endregion

	//Animation setup
	#pragma region SETUP_ANIMATION	
	spriteAnimations = anim;

	currentAnimation = spriteAnimations[0]->animation;

	#pragma endregion

	//UI Setup
	#pragma region SETUP_UI
	if (!font.loadFromFile("font/arial.ttf"))
	{
	}
	points.setFont(font);
	points.setCharacterSize(24);
	points.setString("Points : 100");
	points.setFillColor(sf::Color::White);
	hp.setFont(font);
	hp.setCharacterSize(24);
	hp.setFillColor(sf::Color::White);

	hp.setOrigin(0, 0);
	hp.setString("GAME OVER");
	std::string str = hp.getString();
	sf::Vector2f endPos = hp.findCharacterPos(str.size());
	sf::Vector2f size = ((endPos - hp.getPosition()));
	size.y += hp.getCharacterSize();
	//text.setPosition(0, 0);
	//text.setOrigin(size/2.f + text.getPosition());
	hp.setOrigin(size / 2.f);
	hp.setPosition((getSize() / 2.f + getPosition()));// -getPosition()));

	hp.setString("Health : ");
	#pragma endregion

}
Player::Player() {

}
Player::~Player() {
	delete cursor;
	weaponsInInventory.clear();
}
#pragma endregion

#pragma region GENERAL_PUBLIC_FUNCTIONS
void Player::update(float dt) {
	if (isAlive())
	{
		//Movement and rotation
		move(movementDirection * movementSpeed * dt);
		setRotation(VectorHelper::angleBetweenTwoVectors(upDirection, lookDirection) + Down);

		//Animation
		currentAnimation->animate(dt);	setTextureRect(currentAnimation->getCurrentFrame());

		//Components
		equippedWeapon->update(dt);
		cursor->update(dt);

		//Invincibility
		timeElapsed += dt;
		if (timeElapsed > 300)
		{
			timeElapsed = invincibilityTime + 1;
		}

		//UI

		points.setString("Points : " + std::to_string(static_cast<int>(currency)));
		hp.setString("Health : " + std::to_string(static_cast<int>(health)));
	}

	RepositionUI();
}

void Player::render(Player* player)
{
	//if (isAlive())
	//{
		window->draw(*player); //window->draw(cursor);
	//}


	equippedWeapon->render();
	for (int i = 0; i < tempBullets.size(); i++)	{ window->draw(*tempBullets[i]); }
}

void Player::handleInput(float dt) {
	if (isAlive())
	{
		//Movement, rotation and weapons
		CalculateMovementDirection();
		CalculateLookDirection();
		SwitchWeapon(dt);

		equippedWeapon->handleInput(dt, getPosition(), lookDirection);

		//Animations
		if (equippedWeapon->GetIsShooting() == true) {
			currentAnimation->play();

			//If I had more weapons, I would make.  a difference between automatic,semi-automatic weapons as to trigger these events
			if (equippedWeapon->GetDamage() == 6)//ak47 is 6 dmg, m1911 is 14
			{
				firingType = 2;
			}
			else {
				firingType = 1;
			}
		}
		else {
			firingType = 0;
		}
		if (!currentAnimation->getPlaying())
		{
			currentAnimation->reset();
			
		}
	}

}
#pragma endregion

#pragma region GENERAL_PRIVATE_FUNCTIONS
/// <summary>
/// Calculates the normalised vector of movement input ranging from -1 to 1 on y and x axis.
/// </summary>
void Player::CalculateMovementDirection() {
	#pragma region INPUT
	if (input->isKeyDown(sf::Keyboard::A))
	{
		movementDirection.x = -1;
	}
	else if (input->isKeyDown(sf::Keyboard::D))
	{
		movementDirection.x = 1;
	}
	else {
		movementDirection.x = 0;
	}
	if (input->isKeyDown(sf::Keyboard::W))
	{
		movementDirection.y = -1;
	}
	else if (input->isKeyDown(sf::Keyboard::S))
	{
		movementDirection.y = 1;
	}
	else {
		movementDirection.y = 0;
	}
	#pragma endregion

	movementDirection = VectorHelper::normalise(movementDirection);
}
/// <summary>
/// Calculates the normalised vector between mouse position and player position. Will need to refactor to account for view
/// </summary>
void Player::CalculateLookDirection() {
	sf::Vector2f cursorPosition;
	cursorPosition = sf::Vector2f(cursor->getPosition().x, cursor->getPosition().y);

	lookDirection = VectorHelper::normalise(cursorPosition - getPosition());
}
/// <summary>
/// Changes equippedWeapon to the corresponding button press of weaponsInInventory. Also manages the tempBullets and calls update for tempbullets.
/// </summary>
void Player::SwitchWeapon(float dt) {

	#pragma region Weapon Switching
	if (input->isPressed(sf::Keyboard::Num1))
	{
		PushTempBullets();
		equippedWeapon = weaponsInInventory.front();
		ChooseAnimation(equippedWeapon->GetName());
		//currentAnimation->setPlaying(false);g
	}
	else if (input->isPressed(sf::Keyboard::Num2)) {
		PushTempBullets();
		equippedWeapon = weaponsInInventory.back();
		ChooseAnimation(equippedWeapon->GetName());
		//currentAnimation->setPlaying(false);
	}
	#pragma endregion
	
	UpdateTempBullets(dt);
}

/// <summary>
/// When switching weapons, if any bullets remain alive in the original weapon, push those bullet pointers into the tempBullet pointer vector UNLESS it already exists
/// </summary>
void Player::PushTempBullets() {
	//For all bullets, check if their alive and being rendered
	for (int i = 0; i < equippedWeapon->bullets.size(); i++)
	{
		bool exists = false;
		if (equippedWeapon->bullets[i]->isAlive())
		{
			//if there is any tempBullets that is in the current equippedWeapon(WEAPON BEFORE SWITCHING) then we don't want to duplicate it's existance in tempBullets and we won't add it.
			for (int p = 0; p < tempBullets.size(); p++)
			{
				if (tempBullets[p] == equippedWeapon->bullets[i])	{ exists = true;	break; }
			}

			//If the alive bullets in equippedWeapon, do not exist in tempBullets then put the equippedWeapon.bullets[i] into tempBullets for updating
			if (!exists)	{ tempBullets.push_back(equippedWeapon->bullets[i]); }
		}
	}
}
/// <summary>
/// If tempBullets are alive and the tempBullets do not exist in the equipped weapon, update otherwise erase from vector
/// </summary>
void Player::UpdateTempBullets(float dt) {
	if (!tempBullets.empty())
	{
		//Check all tempbullets
		for (int i = 0; i < tempBullets.size(); i++)
		{
			//If they have died, remove them
			if (tempBullets[i]->isAlive())
			{
				bool exists = false;
				//If the tempBullets[i] exist in the current equippedWeapon.bullets
				for (int p = 0; p < equippedWeapon->bullets.size(); p++)
				{
					if (tempBullets[i] == equippedWeapon->bullets[p])	{ exists = true;	break; }
				}
				
				if (!exists)	{ tempBullets[i]->update(dt); }
				else	{ tempBullets.erase(tempBullets.begin() + i); }
			}
			else {
				tempBullets.erase(tempBullets.begin() + i);
			}

		}
	}
}
#pragma endregion

#pragma region COLLISION_RESPONSE
void Player::collisionResponse(GameObject* go)
{
	//When opening the inventory via weaponBox, updates the ammo counter on the inventory and open inventory
	if (input->isPressed(sf::Keyboard::O))
	{
		for (int i = 0; i < playerInventory.inventorySlots.size(); i++)
		{
			if (playerInventory.inventorySlots[i]->name->text.getString() == "Name:" + weaponsInInventory.front()->GetName())
			{
				playerInventory.inventorySlots[i]->extraText->text.setString("Ammo : " + std::to_string(weaponsInInventory.front()->GetBulletsLeft() + weaponsInInventory.back()->GetBulletsInMag()));
			}
			if (playerInventory.inventorySlots[i]->name->text.getString() == "Name:" + weaponsInInventory.back()->GetName())
			{
				playerInventory.inventorySlots[i]->extraText->text.setString("Ammo : " + std::to_string(weaponsInInventory.back()->GetBulletsLeft() + weaponsInInventory.back()->GetBulletsInMag()));
			}
		}

		playerInventory.OpenClose();
	}

}

void Player::collisionResponse(float damage) {

	invincibilityTime -= timeElapsed;
	timeElapsed = 0;

	if (invincibilityTime < 0 && health > 0)
	{
		health -= damage;
		invincibilityTime = 1;

 		//if health goes below 0, Initiate the endgame sequence
		if (health <= 0)
		{
			health = 0;
			hp.setString("GAME OVER");
			hp.setFillColor(sf::Color::Red);
			setAlive(false);
		}
	}

}

#pragma endregion

void Player::RepositionUI()
{
	points.setPosition(sf::Vector2f(window->getView().getCenter().x - (window->getView().getSize().x / 2.f) + 50.f,
		window->getView().getCenter().y - (window->getView().getSize().y / 2.f) + 26.f));
	hp.setPosition(sf::Vector2f(window->getView().getCenter().x - 50.f,
		window->getView().getCenter().y + (window->getView().getSize().y / 2.f) - 30.f));
}
void Player::AddPoints(float points)
{
	currency += points;
}
void Player::WeaponShopSelection(Weapon* weaponSelected)
{
	//Player doesn't have enough points to purchase
	if (weaponSelected->GetCost() > currency )
	{
		cursor->collisionResponse("Not enough points");
		return;
	}
	currency -= (weaponSelected->GetCost());

	//Player only has one weapon
	if (weaponsInInventory.size() < 2 && weaponsInInventory.front() != weaponSelected)
	{
		weaponsInInventory.push_back(weaponSelected);
		playerInventory.inventorySlots.push_back(new InventorySlots(weaponSelected->GetName(), "Ammo : " + std::to_string(weaponSelected->GetBulletsLeft()), spriteAnimations[0]->textureForAnimation, &font));
		playerInventory.Setup("Inventory", input);
		return;
	}

	//Player has two weapons, check neither are the selected one to replace
	//if (weaponSelected != weaponsInInventory.front() && weaponSelected != weaponsInInventory.back())
	//{
		//ask player which weapon to replace
		//weaponsInInventory.pop_front();
		//weaponsInInventory.push_front(weaponSelected);
		//return;
	//}

	//Player has two weapons, replenish ammo for same weapon
	if (weaponSelected == weaponsInInventory.front())
	{
		weaponsInInventory.front()->SetBulletsLeft(weaponsInInventory.front()->GetMaxBullets());
	}
	else {
		weaponsInInventory.back()->SetBulletsLeft(weaponsInInventory.back()->GetMaxBullets());
	}
	return;
}