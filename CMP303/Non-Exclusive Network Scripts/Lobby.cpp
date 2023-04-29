#include "Lobby.h"
static int clientsConnected = 0;
static bool hosting = false;
static int activeInputField = -1;
Lobby::Lobby(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	////////For some reason debug, must be up here 
	cursor = new Cursor(hwnd, in);
	background = Background(hwnd, in);
	debug = CollisionDebugBox(hwnd, in);
	tcpConnection = Net_TCP::GetInstance();

#pragma region SETUP_UI_ELEMENTS
	UIElement uiElement(UI.font);
	uiElement.setPadding(0, 50, 0, 20);
	int uiElementIndex = 0;
	UI.UIElements.push_back(uiElement);
	UI.UIElements[uiElementIndex].Setup(true, "Start Game", sf::Vector2f(10, 10), sf::Vector2f(200, 50), sf::Color::Magenta, sf::Color::Black, 1.f);
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "Quit", sf::Vector2f(10, 500), sf::Color::Magenta, sf::Color::Black, 1.f);
	
	UI.SetInput(in);
	//UI.NormaliseBoxes(UI.UIElements); 
	UI.CenterAllText(UI.UIElements);
#pragma endregion

#pragma region SETUP_COLLISION_MANAGER
	collisionManager.cursor = cursor;
	collisionManager.clickableUIElements.push_back(&UI.UIElements[0]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[1]);

#pragma endregion

	audio->addSound("sfx/gunshot2.ogg", "Shooting"); //audio->getSound("Shooting").
	audio->addSound("sfx/zombiedeath.ogg", "ZombieDeath");
	audio->addSound("sfx/click.ogg", "Click");
	audio->addMusic("sfx/CMP105.wav", "MenuMusic");




	
}

Lobby::~Lobby()
{
	clean();
}


void Lobby::init() {
	
	initialised = true;
	threads = std::thread(&Lobby::ReceivingCommands, this);
	tcpConnection = Net_TCP::GetInstance();
	clientsConnected = 0;
	tcpConnection->forceLeave = false;
	if (tcpConnection->IsHost())
	{
		collisionManager.clickableUIElements[0]->setAlive(true);
	}
	else {

		collisionManager.clickableUIElements[0]->setAlive(false);
	}
}
//Unfortuanetly due to the way this project was built. I have to manually clean everything instead of doing it by destructor. Because all scenes are created at the start and remain during runtime.
//This allows us to clean any unnecessary mess
void Lobby::clean() {
	initialised = false;
	if (threads.joinable())
	{
		threads.join();
	}
	while (playersUI.size() > 0)
	{
		playersUI.pop_back();
	}
}

void Lobby::handleInput(float dt)
{
#pragma region TEXTFIELD_ENTRY
	if (activeInputField >= 0)
	{
		std::string* fieldTyping = nullptr;
		switch (activeInputField)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}

		if (fieldTyping != nullptr)
		{
			char charPressed = input->getCharDown();
			if (charPressed == -1)
			{
				if (fieldTyping->size() > 0)
					fieldTyping->pop_back();

			}
			else if (charPressed != -2) {
				if (fieldTyping->size() < 10)
					fieldTyping->push_back(charPressed);
			}

		}

	}

	if (input->isKeyDown(sf::Keyboard::A))
	{
		UI.UIElements[1].text.setString(tcpConnection->GetName());

	}
#pragma endregion
	background.handleInput(dt);
}

void Lobby::update(float dt)
{
	if (tcpConnection->forceLeave)
	{
		gameState->setCurrentState(GameState::State::MENU);
		clean();
		tcpConnection->forceLeave = false;
		return;
	}
	if (tcpConnection->startGame)
	{
		clean();
		gameState->setCurrentState(GameState::State::LEVEL);
		return;
	}
	if (audio->getMusic()->getStatus() != audio->getMusic()->Playing) {
		//audio->playMusicbyName("MenuMusic");
	}

	//debug.update(dt);
	background.update(dt);
	cursor->update(dt);
	UI.Update();
	if (background.UpdateUIElements)
	{
		background.setPosition(window->getView().getCenter() - (window->getView().getSize() / 2.f));
		background.overlay.setPosition(window->getView().getCenter() - (window->getView().getSize() / 2.f));
	}
#pragma region COLLISIONCHECKS

	uiResponse = collisionManager.UICollision();

	if (uiResponse >= 0 && uiResponse <= 2)
	{
		audio->playSoundbyName("Click");
	}
	switch (uiResponse)
	{
	case(0):
		//Start Game
		tcpConnection->StartGame();

		clean();
		gameState->setCurrentState(GameState::State::LEVEL);
		break;
	case(1):
		//Quit game
		//Host should continually check if the client is there. If they don't respond x amounts of time, it is assumed they've disconnected and they will be disconnected.
		if (tcpConnection->IsHost())
		{
			//Tell all clients to leave
			tcpConnection->S_CloseLobby();
		}
		else {
			//Disconnect
			tcpConnection->C_LeaveLobby();
		}
		gameState->setCurrentState(GameState::State::MENU);
		clean();
		return;
	case(2):
		break;
	case(3):
		break;
	case (4):
		activeInputField = 0;
		break;
	case (5):
		activeInputField = 1;
		break;
	case (6):
		activeInputField = 2;
		break;
	case (7):
		activeInputField = 3;
		break;
	default:
		break;
	}

	tcpConnection->S_RecievingClients();
	tcpConnection->A_PresenceCheck(dt);

	if (tcpConnection->GetClientCount() - clientsConnected == 1)//Added a client
	{
		AddPlayer(tcpConnection->GetClient(clientsConnected).username);
	}
	else if (tcpConnection->GetClientCount() - clientsConnected == -1) {
		//RemovePlayer();
		playersUI.clear();
		for (int i = 0; i < tcpConnection->GetClientCount(); i++)
		{
			AddPlayer(tcpConnection->GetClient(i).username);

		}
	}
	else if (tcpConnection->GetClientCount() - clientsConnected == 0)
	{
		//No Change
	}
	else {
		std::cout << "Two or more clients somehow got managed at once";
		playersUI.clear();
		for (int i = 0; i < tcpConnection->GetClientCount(); i++)
		{
			AddPlayer(tcpConnection->GetClient(i).username);

		}
	}
	clientsConnected = tcpConnection->GetClientCount();


#pragma endregion
}

void Lobby::render()
{
	beginDraw();
	window->draw(background);
	window->draw(background.overlay);
	UI.Render(window);
	for (int i = 0; i < playersUI.size(); i++)
	{
		playersUI[i].text.setPosition(10, ((i + 1) * 70) + 10);
		window->draw(playersUI[i].text);
	}
	window->draw(*cursor);
	debug.RenderCollisionBoxes();
	endDraw();

}

void Lobby::RemovePlayer() {
	for (int i = 0; i < playersUI.size(); i++)
	{
		bool exists = 0;
		std::string UIName = playersUI[i].text.getString();
		for (int j = 0; j < tcpConnection->GetClientCount(); j++)
		{
			std::string TCPName = "Username: " + tcpConnection->GetClient(j).username;
			if (TCPName == UIName)
				exists = 1;

		}
		//Player name was not found in client list
		if (exists == 0) {
			playersUI.erase(playersUI.begin() + i);
			break;
		}
	}
	//There were two players with the same name
	while (playersUI.size() != tcpConnection->GetClientCount())
	{
		for (int i = 0; i < playersUI.size(); i++)
		{
			int duplicates = 0;
			for (int j = 0; j < playersUI.size(); j++)
			{
				if (playersUI[i].text.getString() == playersUI[j].text.getString())
				{
					duplicates++;
				}
			}
			if (duplicates > 1)
			{
				playersUI.erase(playersUI.begin() + i);
				break;
			}
		}
	}

}
void Lobby::AddPlayer(std::string playerName)
{
	std::string username = "Username: " + playerName;
	UIElement uiElement(UI.font);
	uiElement.setPadding(0, 50, 0, 20);
	uiElement.Setup(false, username, sf::Vector2f(10, 80), sf::Vector2f(200, 50), sf::Color::Magenta, sf::Color::Black, 1.f);
	playersUI.push_back(uiElement);
}

void Lobby::ReceivingCommands() {
	std::cout << "Thread has started";
	while (initialised)
	{
		tcpConnection->A_WaitForMessage();

	}
	
}
