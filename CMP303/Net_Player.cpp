#include "Net_Player.h"
static float ping = 0;
static int totalPackets = 0;
static UINT8 currentNum = 1;
static float timeState = 0;
static sf::Vector2f savedOriginalPosition;
static sf::Vector2f savedOriginalRotation;
Net_Player::Net_Player(sf::Vector2f size, std::vector<AnimationStruct*> anim, sf::RenderWindow* win) {
	Sprites::SetupEntity(0, 0, size, anim[0]->textureForAnimation);
	setTextureRect(sf::IntRect(0, 0, 300, 512));
	setAlive(true);
	setOrigin(23, 25);
	window = win;
	spriteAnimations = anim;
	currentAnimation = spriteAnimations[0]->animation;

	weapon0 = new M1911(win);
	weapon1 = new AK47(win);
	equippedWeapon = weapon0;
	savedOriginalPosition = getPosition();
}

void Net_Player::CalculateRotation(sf::Vector2f lookDir)
{
	setRotation(VectorHelper::angleBetweenTwoVectors(upDirection, lookDir) + Down);
	lookDirection = lookDir;
}

void Net_Player::update(float dt) {
	if (isAlive())
	{
		MoveToNewestPoint(dt);
		//Animation
		currentAnimation->animate(dt);	setTextureRect(currentAnimation->getCurrentFrame());

		//Components
		for (int i = 0; i < equippedWeapon->bullets.size(); i++) {
			if (equippedWeapon->bullets[i]->isAlive()) {
				equippedWeapon->bullets[i]->update(dt);
				//deathCheck(i);
			}
		}
		UpdateTempBullets(dt);

	}

}
void Net_Player::Render(sf::RenderWindow* window) {
	window->draw(*this); //window->draw(cursor);
	//}

	equippedWeapon->render();
	for (int i = 0; i < tempBullets.size(); i++) { window->draw(*tempBullets[i]); }

}

void Net_Player::Fire() {
	if (fireMode == 1)
		fireMode = 0;

	equippedWeapon->Reload();
	equippedWeapon->spawn(getPosition(), lookDirection);
}

void Net_Player::SwitchWeapon(int weapon) {

	PushTempBullets();

	if (weapon == 0)
		equippedWeapon = weapon0;
	else if (weapon == 1)
		equippedWeapon = weapon1;

	ChooseAnimation(equippedWeapon->GetName());
}
void Net_Player::UpdateTempBullets(float dt) {
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
					if (tempBullets[i] == equippedWeapon->bullets[p]) { exists = true;	break; }
				}

				if (!exists) { tempBullets[i]->update(dt); }
				else { tempBullets.erase(tempBullets.begin() + i); }
			}
			else {
				tempBullets.erase(tempBullets.begin() + i);
			}

		}
	}
}

void Net_Player::PushTempBullets() {
	//For all bullets, check if their alive and being rendered
	for (int i = 0; i < equippedWeapon->bullets.size(); i++)
	{
		bool exists = false;
		if (equippedWeapon->bullets[i]->isAlive())
		{
			//if there is any tempBullets that is in the current equippedWeapon(WEAPON BEFORE SWITCHING) then we don't want to duplicate it's existance in tempBullets and we won't add it.
			for (int p = 0; p < tempBullets.size(); p++)
			{
				if (tempBullets[p] == equippedWeapon->bullets[i]) { exists = true;	break; }
			}


			//If the alive bullets in equippedWeapon, do not exist in tempBullets then put the equippedWeapon.bullets[i] into tempBullets for updating
			if (!exists) { tempBullets.push_back(equippedWeapon->bullets[i]); }
		}
	}
}

void Net_Player::Interpolate(float dt)
{

	sf::Vector2f newestPosition = savedStates.back().position;
	sf::Vector2f newestRotation = savedStates.back().lookDirection;

	sf::Vector2f diff = (newestRotation - savedOriginalRotation);
	float percent = timeState / 0.1f;
	float currentPercentage = VectorHelper::lerp(0, 1, percent);
	sf::Vector2f newLookDirection = savedOriginalRotation + (diff * currentPercentage);//We add 0.1f because that is the network tick


	//sf::Vector2f diff = newestRotation - lookDirection;
	//sf::Vector2f newLookDirection = lookDirection + (diff * ((dt + 0.1f)));

	CalculateRotation(newLookDirection);

	//sf::Vector2f direction = VectorHelper::normalise(newestPosition - getPosition());
	//float mag = VectorHelper::magnitude(newestPosition - getPosition());

	//move(direction * ((dt * 10.0f) + 0.1f));//Multiplying by 100.0f works better than adding

	//Linear interpolation by 1 second. We will reach direction position in 1 second
	//move(direction * dt);
	//sf::Vector2f direction = VectorHelper::normalise(newestPosition - savedOriginalPosition);
	//float mag = VectorHelper::magnitude(newestPosition - getPosition());
	//float mag = VectorHelper::magnitude(newestPosition - savedOriginalPosition);
	//sf::Vector2f direction = (newestPosition - getPosition());
	sf::Vector2f direction = (newestPosition - savedOriginalPosition);
	float mag = VectorHelper::magnitude(newestPosition - savedOriginalPosition);
	sf::Vector2f direction2 = (newestPosition - getPosition());
	float pinginterval = ping / totalPackets;
	//move(direction * ((((dt + (0.1f*dt)) / (pinginterval + 0.1f)) * mag)));
	move((direction * (dt / (pinginterval + 0.1f))) + (direction2 * (dt)));


	/*float percent = timeState / 0.1f;
	float currentPercentage = VectorHelper::lerp(0, 1, percent);
	std::cout << currentPercentage << std::endl;
	setPosition(savedOriginalPosition + (newestPosition * mag * currentPercentage));*/

	if (VectorHelper::magnitude(getPosition()) - VectorHelper::magnitude(newestPosition) > 300)
	{
		setPosition(newestPosition);
	}

}

void Net_Player::Prediction(float dt)
{
	sf::Vector2f pos0= savedStates[0].position;
	sf::Vector2f pos1 = savedStates[1].position;
	sf::Vector2f pos2 = savedStates[2].position;

	sf::Vector2f direction = VectorHelper::normalise(pos2 - pos1);
	float mag = VectorHelper::magnitude(pos2 - pos1);
	move(direction * ((dt) * mag));//Multiplying by 100.0f works better than adding

}


void Net_Player::MoveToNewestPoint(float dt)
{
	if (savedStates.size() < 3) {
		savedOriginalPosition = getPosition();
		return;

	}


	UINT8 stateNum = savedStates.back().stateNumber;

	sf::Vector2f firstPos = savedStates[2].position;
	sf::Vector2f firstRot = savedStates[2].lookDirection;
	UINT8 firstNum = savedStates[2].stateNumber;

	if (currentNum != stateNum)
	{
		//We have missed a state
		if (currentNum - stateNum > 1)
		{
			setPosition(firstPos);
			CalculateRotation(lookDirection);
		}
		ping += 1.0f - timeState;
		totalPackets++;
		timeState = 0;
		currentNum = stateNum;
		savedOriginalPosition = getPosition();
		savedOriginalRotation = lookDirection;
	}
	else {
		timeState += dt;

	}


	//Interpolate(dt);
	if (timeState <= 0.1)
	{
		Interpolate(dt);
		//Prediction(dt);

	}
	else {
		Prediction(dt);
	}



}
