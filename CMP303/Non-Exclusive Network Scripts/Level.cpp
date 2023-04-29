#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;



	#pragma region INITIALISE_OBJECTS
	debugCollision = CollisionDebugBox(window, in);
	background = Background(window, input);

	availableWeaponsInLevel.push_back(new M1911(in, window)); availableWeaponsInLevel.push_back(new AK47(in, window));
	availableWeaponsInLevel[0]->SetWeaponImage(animationStorage.playerAnimation[0]->textureForAnimation); availableWeaponsInLevel[1]->SetWeaponImage(animationStorage.playerAnimation[1]->textureForAnimation);
	weaponsBox = new WeaponsBox(availableWeaponsInLevel, sf::Vector2f(1250, 730), sf::Vector2f(250, 200), in, window);


	player = new Player(50, 100, sf::Vector2f(55, 108), 55, 55, animationStorage.playerAnimation, in, window, sf::Vector2f(1000, 1000), availableWeaponsInLevel[0]);
	enemyManager.Setup(window, &animationStorage.walkerAnimation, &animationStorage.armouredAnimation);
	EnemyBase::FindPlayer(player);

	deathObject.push_back(new GameObject()); deathObject[0]->setCollider(true); deathObject[0]->setPosition(440, 1060); deathObject[0]->setCollisionBox(0, 0, 200, 200); deathObject[0]->setAlive(true);
	immovableObject.push_back(new GameObject()); immovableObject[0]->setCollider(true); immovableObject[0]->setPosition(700, 400); immovableObject[0]->setCollisionBox(0, 0, 200, 200); 	immovableObject[0]->setAlive(true);
	immovableObject.push_back(new GameObject()); immovableObject[1]->setCollider(true); immovableObject[1]->setPosition(1280, 770); immovableObject[1]->setCollisionBox(0, 0, 180, 110); 	immovableObject[1]->setAlive(true);


	#pragma endregion

	#pragma region SETUP_UI_MANAGER
	UI.PauseMenu.push_back(UIElement(UI.font));
	UI.PauseMenu[0].Setup(false, "", window->getView().getSize() / 2.f, sf::Vector2f(500, 500), sf::Color::Black);
	UI.PauseMenu[0].setOrigin(UI.PauseMenu[0].getSize() / 2.f);
	UI.PauseMenu.push_back(UIElement(UI.font));
	UI.PauseMenu[1].Setup(true, "Continue", UI.PauseMenu[0].getPosition() + sf::Vector2f(-200, -200), sf::Color::Red);
	UI.PauseMenu.push_back(UIElement(UI.font));
	UI.PauseMenu[2].Setup(true, "Exit", UI.PauseMenu[0].getPosition() + sf::Vector2f(-200, -100), sf::Color::Red);


	UI.SetInput(in);
	#pragma endregion

	#pragma region SETUP_DEBUG
		//Adds all Enemy objects inside enemyManager to the debug.
		#pragma region ALL_ENEMIES
		
		for (int i = 0; i < enemyManager.zombieWalker.size(); i++)
		{
			debugCollision.AddCollisionBox(&enemyManager.zombieWalker[i]);
		}

		for (int i = 0; i < enemyManager.zombieArmoured.size(); i++)
		{
			debugCollision.AddCollisionBox(&enemyManager.zombieArmoured[i]);
		}
		#pragma endregion

		//Adds player and cursor to debug
		#pragma region PLAYER_AND_PLAYERCOMPONENTS
		debugCollision.AddCollisionBox(player);
		debugCollision.AddCollisionBox(player->cursor);

		#pragma endregion	

		//Adds PauseMenu, Inventories to the debug.
		#pragma region UI
		//Pause
		for (int i = 0; i < UI.PauseMenu.size(); i++)
		{
			debugCollision.AddCollisionBox(&UI.PauseMenu[i]);
		}
		//Inventories
		//Doing it seperately for collisionManager reasons
		debugCollision.AddCollisionBox(player->playerInventory.leftArrow); 		debugCollision.AddCollisionBox(player->playerInventory.rightArrow);
		debugCollision.AddCollisionBox(weaponsBox->weaponBoxInventory.leftArrow); 		debugCollision.AddCollisionBox(weaponsBox->weaponBoxInventory.rightArrow);
		for (int i = 0; i < player->playerInventory.inventorySlots.size(); i++)
		{
			debugCollision.AddCollisionBox(player->playerInventory.inventorySlots[i]->boundingBox);
		}
		for (int i = 0; i < weaponsBox->weaponBoxInventory.inventorySlots.size(); i++)
		{
			debugCollision.AddCollisionBox(weaponsBox->weaponBoxInventory.inventorySlots[i]->boundingBox);
		}

		#pragma endregion

		//MapCollision
		#pragma region Map
		for (auto& tile : *map.getLevel())
		{
			if (tile.isCollider())
			{
				debugCollision.AddCollisionBox(&tile);
			}
		}

		for (int i = 0; i < deathObject.size(); i++)
		{
			debugCollision.AddCollisionBox(deathObject[i]);
		}
		for (int i = 0; i < immovableObject.size(); i++)
		{
			debugCollision.AddCollisionBox(immovableObject[i]);
		}

		#pragma endregion

		//WeaponsBox
		debugCollision.AddCollisionBox(weaponsBox);

		//Adds all bullets in availableWeaponsInLevel
		#pragma region BULLETS
		for (int i = 0; i < availableWeaponsInLevel.size(); i++)
		{
			for (int p = 0; p < (availableWeaponsInLevel[i]->bullets.size()); p++)
			{
				debugCollision.AddCollisionBox(availableWeaponsInLevel[i]->bullets[p]);
			}
		}
		#pragma endregion
	#pragma endregion

	#pragma region SETUP_COLLISION_MANAGER	
	//Player, enemyManager, cursor
	collisionManager.player = player; collisionManager.enemyManager = &enemyManager; collisionManager.cursor = player->cursor;
	
	//clickableUI, remember vector arrangement for switch statement use in UI collision
	for (int i = 1; i < UI.PauseMenu.size(); i++) {	collisionManager.clickableUIElements.push_back(&UI.PauseMenu[i]); }

	//Map
	for (auto& tile : *map.getLevel())
	{
		if (tile.isCollider())
		{
			collisionManager.mapBounds.push_back(&tile);
		}
	}
	collisionManager.deathObjects = deathObject;
	collisionManager.immovableObjects = immovableObject;
	collisionManager.weaponBox = weaponsBox;

	//Inventories, doing this individually allows us to control the switch statements better.
	collisionManager.inventoryClicks.push_back(player->playerInventory.leftArrow);	collisionManager.inventoryClicks.push_back(player->playerInventory.rightArrow);
	collisionManager.inventoryClicks.push_back(weaponsBox->weaponBoxInventory.leftArrow); 	collisionManager.inventoryClicks.push_back(weaponsBox->weaponBoxInventory.rightArrow);
	for (int i = 0; i < weaponsBox->weaponBoxInventory.inventorySlots.size(); i++)
	{
		collisionManager.inventoryClicks.push_back(weaponsBox->weaponBoxInventory.inventorySlots[i]->boundingBox);
	}
	#pragma endregion

	#pragma region Audio

	for (int i = 0; i < availableWeaponsInLevel.size(); i++)
	{
		availableWeaponsInLevel[i]->aud = aud;
	}
	enemyManager.aud = aud;


	#pragma endregion
	player->setPosition(1000, 1000);
	exiting = 0;
	
	
}

Level::~Level()
{
	clean();
	delete player;
	delete weaponsBox;
	for (int i = 0; i < availableWeaponsInLevel.size(); i++)
	{
		delete availableWeaponsInLevel[i];
		availableWeaponsInLevel[i] = nullptr;
	}
	for (int i = 0; i < deathObject.size(); i++)
	{
		delete deathObject[i];
		deathObject[i] = nullptr;
	}
	for (int i = 0; i < immovableObject.size(); i++)
	{
		delete immovableObject[i];
		immovableObject[i] = nullptr;
	}
	delete background.backgroundTexture;
	delete background.overlayTexture;
}

void Level::init() {
	initialised = true;
	tcpConnection = Net_TCP::GetInstance();
	threads = std::thread(&Level::ReceivingCommands, this);
	////The ordering doesn't actually matter right now
	player->setPosition(tcpConnection->GetClient(0).state.position);
	for (int i = 1; i < tcpConnection->GetClientCount(); i++)
	{
		clientPlayers[tcpConnection->GetClient(i).identifier] = new Net_Player(sf::Vector2f(55, 108), animationStorage.playerAnimation, window);
		clientPlayers[tcpConnection->GetClient(i).identifier]->setPosition(tcpConnection->GetClient(i).state.position);
		clientPlayers[tcpConnection->GetClient(i).identifier]->setAlive(true);
		clientPlayers[tcpConnection->GetClient(i).identifier]->weapon0->aud = audio;
		clientPlayers[tcpConnection->GetClient(i).identifier]->weapon1->aud = audio;
	}
	players.push_back(player);
	for (int i = 1; i < tcpConnection->GetClientCount(); i++)
	{
		players.push_back(clientPlayers[tcpConnection->GetClient(i).identifier]);

	}
	srand(tcpConnection->eventIndicesInformation.front());
	tcpConnection->eventIndicesInformation.pop();
	player->updateID = 150;
}
void Level::clean() {
	initialised = false;
	if (threads.joinable())
	{
		threads.join();
	}
	for(int i = 1; i < tcpConnection->GetClientCount(); i++)
	{
		delete clientPlayers[tcpConnection->GetClient(i).identifier];
	}

	if (tcpConnection->IsHost())
	{
		//Tell all clients to leave
		tcpConnection->S_CloseLobby();
	}
	else {
		//Disconnect
		tcpConnection->C_LeaveLobby();
	}
}

void Level::handleInput(float dt)
{
	//Pause Restricted
	if (!UI.pauseMenuOn && !player->playerInventory.openClose)
	{
		player->handleInput(dt);
	}
	else {
		player->cursor->update(dt);
	}

	//To Pause game
	if (input->isPressed(sf::Keyboard::Escape))
	{
		UI.ShowPauseMenu(window);
	}
}

void Level::update(float dt)
{
	
	//Pause Restricted
	if (!UI.pauseMenuOn)
	{
		//Inventory Restricted
		if (!weaponsBox->weaponBoxInventory.openClose)
		{
			player->update(dt);
		}

		enemyManager.update(dt, players);
		//Collision udpate
		collisionManager.EnemyCollisions();
		collisionManager.MapCollisions(dt);
	}
	background.update(dt, player->getPosition());//Moves with player
	
	
	//if Window Resizes, update UI things
	if (background.UpdateUIElements)
	{
		enemyManager.RepositionUI();
		player->equippedWeapon->RepositionUI();
	}


	#pragma region UI_COLLISION
	//Pause Collision
	if (UI.pauseMenuOn)
	{
		UI.Update();
		uiResponse = collisionManager.UICollision();
		switch (uiResponse)
		{
		case(0):
			audio->playSoundbyName("Click");
			UI.pauseMenuOn = !UI.pauseMenuOn;
			break;
		case(1):
			audio->playSoundbyName("Click");
			gameState->setCurrentState(GameState::State::PAUSE);
			break;
		default:
			break;
		}
	}
	//Inventory Collision
	if (!UI.pauseMenuOn && player->playerInventory.openClose)
	{
		weaponsBox->weaponBoxInventory.Reposition(window->getView().getCenter() - (window->getView().getSize() / 2.f));
		player->playerInventory.Reposition(weaponsBox->weaponBoxInventory.container->getPosition() + sf::Vector2f(0,weaponsBox->weaponBoxInventory.container->getSize().y + 10));
		uiResponse = collisionManager.InventoryCollision();
		if (uiResponse >= 0 && uiResponse <= 5)
		{
			audio->playSoundbyName("Click");
		}
		switch (uiResponse)
		{
		case(0):
			//The arrows for player inventories shouldn't exist, YET. They are room for future, if we decide to allow more than 2 weapons in inventory
			break;
		case(1):
			break;
		case(2):
			weaponsBox->weaponBoxInventory.PreviousPage();
			break;
		case(3):
			weaponsBox->weaponBoxInventory.NextPage();
			break;
		case(4):
			player->WeaponShopSelection(weaponsBox->availableWeapons[0]);
			break;
		case(5):
			player->WeaponShopSelection(weaponsBox->availableWeapons[1]);
			break;
		default:
			break;
		}
	}

#pragma endregion

	debugCollision.update(dt);

	if (!player->isAlive())
	{
		exiting += dt;
		//player->hp.setCharacterSize(24 + exiting * 5);
		player->hp.setScale(sf::Vector2f(exiting + 1, exiting + 1));
		player->hp.setPosition(window->getView().getCenter());
		if (exiting > 5)
		{
			gameState->setCurrentState(GameState::State::PAUSE);
			
			//Show Gameover text
		}
	}

#pragma region NETWORKED

	Net_EventTrigger();//Checks if there were any events
	Net_SendEnemyDamage();//If any enemies were damaged in the last frame, send the event
	

	for (int i = 1; i < tcpConnection->GetClientCount(); i++)
	{
		//Movement
		sf::Vector2f newPosState = tcpConnection->GetClient(i).state.position;
		sf::Vector2f newRotState = tcpConnection->GetClient(i).state.lookDirection;
		UINT8 newIDState = tcpConnection->GetClient(i).state.stateNumber;
		Net_UserState newState(newPosState, newRotState, newIDState);
		clientPlayers[tcpConnection->GetClient(i).identifier]->AddNewestState(newState);

		clientPlayers[tcpConnection->GetClient(i).identifier]->update(dt);


		collisionManager.EnemyClientBulletCollision(clientPlayers[tcpConnection->GetClient(i).identifier]);

		//Firing
		if (clientPlayers[tcpConnection->GetClient(i).identifier]->fireMode != 0)
		{
			clientPlayers[tcpConnection->GetClient(i).identifier]->Fire();
		}

	}
	networkTick += dt;
	slowNetworkTick += dt;
	//Currently every tick we send the users position and rotation
	if (networkTick > networkInterval)
	{
		networkTick = 0;
		Net_UserState newState;
		newState.position = player->getPosition();
		newState.lookDirection = player->getLookDirection();
		newState.stateNumber = UINT8(player->updateID);
		player->updateID++;
		if (player->updateID == 100)
		{
			player->updateID = 1;
		}
		
		tcpConnection->UpdateState(newState);

	}
	//Every so often, make clients verify and rectify that their enemies are chasing the same players as on the host side
	if (slowNetworkTick > slowNetworkInterval)
	{
		slowNetworkTick = 0;
		if (tcpConnection->IsHost())
		{
			Net_SendEnemyConsistancy();
		}
	}
	//If we change weapons, send that as an event
	if (player->firingType != 0)
	{
		sf::Packet firePack;
		UINT8 firingType = player->firingType;
		UINT8 identity = tcpConnection->GetClient(0).identifier;
		firePack << COM_EGAMEFIREMODE << identity << firingType;
		tcpConnection->SendMessage(firePack);
	}

#pragma endregion
}
void Level::Net_SendEnemyConsistancy() {
	//Send all the alive enemies ID and their target ID
	//We translate the targetNumber to ID, because the targetNumber will mean different clients on each machine.
	sf::Packet enemyPacket;
	enemyPacket << COM_EGAMEENEMYCONSISTANCY;
	enemyPacket << enemyManager.aliveEnemies.size();
	for (int i = 0; i < enemyManager.aliveEnemies.size(); i++)
	{
		enemyPacket << enemyManager.aliveEnemies[i]->Net_EnemyIdentifier;
		enemyPacket << tcpConnection->GetClient(enemyManager.aliveEnemies[i]->targetNumber).identifier;
		enemyPacket << enemyManager.aliveEnemies[i]->getPosition().x;
		enemyPacket << enemyManager.aliveEnemies[i]->getPosition().y;
	}
	tcpConnection->SendMessage(enemyPacket);
}
void Level::Net_RecieveEnemyConsistancy() {
	//Grabs all enemies and their target identifier, translate it back into the target and verify that they are chasing the same targets locally
	UINT8 enemyAmount = tcpConnection->eventIntInformation.front();
	tcpConnection->eventIntInformation.pop();
	std::vector<EnemyBase*> enemies = enemyManager.aliveEnemies;//Copying the original vector so we can shrink it when we rectify/validate an enemy, reducing the run time

	//We check for enemyAmount and not enemyManager.aliveEnemies.size() 
	//because if we killed an enemy while this has been sending, we will end up leaving a few ints in the eventIntInformation
	for (int i = 0; i < enemyAmount; i+=2)
	{
		UINT8 enemyId, playerId;
		enemyId = tcpConnection->eventIntInformation.front();
		tcpConnection->eventIntInformation.pop();
		playerId = tcpConnection->eventIntInformation.front();
		tcpConnection->eventIntInformation.pop();
		float x, y;
		x = tcpConnection->eventIntInformation.front();
		tcpConnection->eventIntInformation.pop();
		y = tcpConnection->eventIntInformation.front();
		tcpConnection->eventIntInformation.pop();
		sf::Vector2f positionFix = sf::Vector2f(x, y);

		bool exist = false;
		for (int j = 0; j < enemies.size(); j++)
		{
			if (enemies[j]->Net_EnemyIdentifier != enemyId)
				continue;
			exist == true;
			//If enemy is not chasing same client, rectify that by translating information back
			if(tcpConnection->GetClient(enemies[j]->targetNumber).identifier != playerId)
			{
				enemies[j]->target = (clientPlayers[playerId]);
				for (int k = 0; k < players.size(); k++)
				{
					if (players[k] == enemies[j]->target)
					{
						enemies[j]->targetNumber = k;
						//Unsure if this SHOULD be replaced with a interpolation method
						enemies[j]->setPosition(positionFix);
						break;
					}
				}
			}
			enemies.erase(enemies.begin() + j);
		}
		//If the enemy does not exist on our end, we need to spawn it
		if (exist == false)
		{
			enemyManager.spawn(enemyId, positionFix);
		}
		//If the enemy exists on our end but the host does not have it, we need to destroy it//This is not currently easily possible.
		//Server authoritive damage would've made it possible.
	}
}
void Level::Net_SendEnemyDamage() {
	//Every frame, check what enemies have been damaged, send the damange information over
	sf::Packet packet;
	UINT8 enemysize = collisionManager.Net_EnemyDamage.size();
	if (!tcpConnection->IsHost())
	{
		UINT8 id = tcpConnection->GetClient(0).identifier;
		packet << UINT8(COM_CLIENTCALL) << id;

	}
	packet << UINT8(COM_EGAMEDAMAGEENEMY) << enemysize;
	bool send = false;
	for (int i = 0; i < enemysize; i += 2)
	{
		UINT8 enemyIdentity = collisionManager.Net_EnemyDamage.front();
		collisionManager.Net_EnemyDamage.pop();
		UINT8 bulletDamage = collisionManager.Net_EnemyDamage.front();
		collisionManager.Net_EnemyDamage.pop();

		packet << enemyIdentity << bulletDamage;
		send = true;
	}

	if (send)
	{
		tcpConnection->SendMessage(packet);
	}
}

void Level::ChangeClientFire() {
	//Grabs client and their firemode, then changes it locally
	UINT8 clientIdentifier = tcpConnection->eventIntInformation.front();
	tcpConnection->eventIntInformation.pop();
	UINT8 fireMode = tcpConnection->eventIntInformation.front();
	tcpConnection->eventIntInformation.pop();

	for (int i = 0; i < tcpConnection->GetClientCount(); i++)
	{
		if (tcpConnection->GetClient(i).identifier == clientIdentifier)
		{
			clientPlayers[clientIdentifier]->fireMode = fireMode;
		}
	}
}
void Level::Net_RecieveEnemyDamage() {
	//Grabs all enemies that have been damaged on the network and applies the damage locally
	UINT8 amount = tcpConnection->eventIntInformation.front();
	tcpConnection->eventIntInformation.pop();

	for (int i = 0; i < amount; i += 2)
	{
		UINT8 enemyIdentity = tcpConnection->eventIntInformation.front();
		tcpConnection->eventIntInformation.pop();
		UINT8 bulletDamage = tcpConnection->eventIntInformation.front();
		tcpConnection->eventIntInformation.pop();


		//If the enemy is already dead, we won't damage it
		for (int p = 0; p < enemyManager.aliveEnemies.size(); p++)
		{
			if (enemyManager.aliveEnemies[p]->Net_EnemyIdentifier == enemyIdentity)
			{
				enemyManager.aliveEnemies[p]->TakeDamage(bulletDamage);
			}
		}

	}

}
void Level::Net_EventTrigger() {
	//An event based system where we check if there are any event in our buffers,
	//If there were check the event and each function will use the corresponding buffers i.e. the int/float/indices buffer to translate all that raw information into useful information
	while (tcpConnection->eventCommands.size() > 0)
	{
		UINT8 com = tcpConnection->eventCommands.front();

			switch (com)
			{
			case UINT8(COM_EGAMEFIREMODE):
				ChangeClientFire();
				break;
			case UINT8(COM_EGAMEDAMAGEENEMY):
				Net_RecieveEnemyDamage();
				break;
			case UINT8(COM_EGAMEENEMYCONSISTANCY):
				Net_RecieveEnemyConsistancy();
				break;
			default:

				std::cout << "CheckCommand: Recieved unknown event command" << std::endl;

				break;
			}
		tcpConnection->eventCommands.pop();

	}
	

}
// Render level
void Level::render()
{
	beginDraw();
	//Sprites

	window->draw(background);
	player->render(player);
	for (int i = 1; i < tcpConnection->GetClientCount(); i++)
	{
		clientPlayers[tcpConnection->GetClient(i).identifier]->Render(window);
	}
	enemyManager.render(window);
	window->draw(background.overlay);

	window->draw(player->points);
	window->draw(enemyManager.roundCounter);
	window->draw(player->equippedWeapon->magazine);
	window->draw(player->equippedWeapon->reload);
	window->draw(player->hp);

	player->playerInventory.Render(window);
	weaponsBox->weaponBoxInventory.Render(window);


	UI.Render(window);

	window->draw(*player->cursor);
	window->draw(*player->cursor->GetHoverText());
	//Debug //Press P to turn on
	//debugCollision.RenderCollisionBoxes();

	endDraw();
}
void Level::ReceivingCommands() {
	std::cout << "Thread has started";
	while (initialised)
	{
		tcpConnection->A_WaitForMessage();

	}

}
