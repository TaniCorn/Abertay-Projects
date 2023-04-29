#ifndef NET_PLAYER_H
#define NET_PLAYER_H

#include "States.h"
#include "Sprites.h"
#include "M1911.h"
#include "AK47.h"

class Net_Player : public Sprites {
public:
	Net_Player();
	Net_Player( sf::Vector2f size, std::vector<AnimationStruct*> anim, sf::RenderWindow* win);

	void CalculateRotation(sf::Vector2f lookDir);

	void update(float dt);
	void Render(sf::RenderWindow* win);

	void Fire();
	void SwitchWeapon(int weapon);
	void UpdateTempBullets(float dt);
	void PushTempBullets();
	void Interpolate(float dt);
	void Prediction(float dt);
	AK47* weapon1;
	M1911* weapon0;
	std::vector<Bullet*> tempBullets;

	void AddNewestState(Net_UserState state) {

		if (savedStates.size() < 3) {
			savedStates.push_back(state);
			return;
		}
		if (int(state.stateNumber) != int(savedStates.back().stateNumber)){

			//if (state.stateNumber - savedStates.back().stateNumber > 3 && state.stateNumber < 95)//If we have missed around 3+ packets. Delete all current states and start from a new point
			//{
			//	savedStates.clear();
			//	savedStates.push_back(state);
			//	return;
			//}

			//Basic
			savedStates.erase(savedStates.begin());
			savedStates.push_back(state);
			return;
		}
		else {
			//duplicated state
			return;
		}

		std::cout << "New state Unhandled";

		
	}

private:
	std::vector<Net_UserState> savedStates;
	void MoveToNewestPoint(float dt);
public:
	int fireMode;
	Weapon* equippedWeapon;//The current weapon the player is using in their hand. Used to shoot. 
};

#endif // !NET_PLAYER_H
