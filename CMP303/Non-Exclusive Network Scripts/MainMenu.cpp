#include "MainMenu.h"

MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	////////For some reason debug, must be up here 
	cursor = new Cursor(hwnd, in);
	background = Background(hwnd, in);
	debug = CollisionDebugBox(hwnd, in);

#pragma region SETUP_UI_ELEMENTS
	UIElement uiElement(UI.font);
	uiElement.setPadding(0, 50, 0, 20);
	int uiElementIndex = 0;
	UI.UIElements.push_back(uiElement);
	UI.UIElements[uiElementIndex].Setup(true, "Host Game", sf::Vector2f(10, 10), sf::Vector2f(50, 50), sf::Color::Magenta, sf::Color::Black, 1.f);
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "Join Game", sf::Vector2f(220, 10), sf::Vector2f(50, 50), sf::Color::Magenta, sf::Color::Black, 1.f);
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "How To Play", sf::Vector2f(10, 80), sf::Color::Magenta, sf::Color::Black, 1.f);
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "Quit", sf::Vector2f(10, 150), sf::Color::Magenta, sf::Color::Black, 1.f);
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(false, "IP Address:", sf::Vector2f(430, 10));	
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(false, "TCP Port:", sf::Vector2f(430, 80));		
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(false, "Player Name:", sf::Vector2f(430, 150));	
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "Enter IP", sf::Vector2f(640, 10), sf::Vector2f(50,50));	
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "Enter TCP Port", sf::Vector2f(640, 80), sf::Vector2f(50, 50));	
	UI.UIElements.push_back(uiElement);
	UI.UIElements[++uiElementIndex].Setup(true, "Enter Your Name", sf::Vector2f(640, 150), sf::Vector2f(50, 50));
	UI.SetInput(in); 
	UI.NormaliseBoxes(UI.UIElements); UI.CenterAllText(UI.UIElements);
	#pragma endregion

	#pragma region SETUP_COLLISION_MANAGER
	collisionManager.cursor = cursor;
	collisionManager.clickableUIElements.push_back(&UI.UIElements[0]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[1]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[2]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[3]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[7]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[8]);
	collisionManager.clickableUIElements.push_back(&UI.UIElements[9]);
	#pragma endregion
	
	audio->addSound("sfx/gunshot2.ogg", "Shooting"); //audio->getSound("Shooting").
	audio->addSound("sfx/zombiedeath.ogg", "ZombieDeath");
	audio->addSound("sfx/click.ogg", "Click");
	audio->addMusic("sfx/CMP105.wav", "MenuMusic");
	initialised = false;



	tcpConnection = new Net_TCP();

}

MainMenu::~MainMenu()
{
}
void MainMenu::init() {
	initialised = true;

}
void MainMenu::clean() {
	initialised = false;
}
static int activeInputField = -1;

void MainMenu::handleInput(float dt)
{
	#pragma region TEXTFIELD_ENTRY
	if (activeInputField >= 0)
	{
		std::string* fieldTyping = nullptr;
		switch (activeInputField)
		{
		case 0:
			fieldTyping = &ipTyped;
			break;
		case 1:
			fieldTyping = &tcpPortTyped;
			break;
		case 2:
			fieldTyping = &playerNameTyped;
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
#pragma endregion
	background.handleInput(dt);
}
static bool hosting = false;
void MainMenu::update(float dt)
{

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
		//Host a game
		std::cout << "You are now hosting";
		//udpConnection->SetHost(true);
		tcpConnection->SetHost(true);
		tcpConnection->SetName(GetTypedName());
		tcpConnection->GetClient(0).username = tcpConnection->GetName();
		hosting = true;
		tcpConnection->StartListener();
		//We will start to recieve clients once we are in the Lobby menu
		gameState->setCurrentState(GameState::State::LOBBY);
		clean();
		break;
	case(1):
		//Join a game

		//udpConnection->SetHost(false);
		tcpConnection->SetHost(false);
		tcpConnection->SetName(GetTypedName());
		tcpConnection->GetClient(0).username = tcpConnection->GetName();

		if (!tcpConnection->C_ConnectToHost(ipTyped, tcpPortTyped))
		{
			std::cout << "Unable to establish a TCP Connection";
			break;
		}
		// Make TCP responsible for handling the connection
		/*if (!udpConnection->ConnectToHost(ipTyped, std::to_string(tcpConnection->udpPort)))
		{
			std::cout << "Unable to establish a UDP Connection";
			break;
		}*/
		clean();
		gameState->setCurrentState(GameState::State::LOBBY);
		break;
	case(2):
		gameState->setCurrentState(GameState::State::EXIT);
		break;
	case(3):
		gameState->setCurrentState(GameState::State::EXIT);
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
	if (ipTyped.size() > 0)
		collisionManager.clickableUIElements[4]->text.setString(ipTyped);
	else
		collisionManager.clickableUIElements[4]->text.setString("Enter IP");

	if (tcpPortTyped.size() > 0)
		collisionManager.clickableUIElements[5]->text.setString(tcpPortTyped);
	else
		collisionManager.clickableUIElements[5]->text.setString("Enter TCP Port");	
	
	
	if (playerNameTyped.size() > 0)
		collisionManager.clickableUIElements[6]->text.setString(playerNameTyped);
	else 
		collisionManager.clickableUIElements[6]->text.setString("Enter Your Name");


	/*if (hosting)
	{
		udpConnection->RecievingClients();
		tcpConnection->RecievingClients();
	}*/


	#pragma endregion
}

void MainMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(background.overlay);
	UI.Render(window);
	window->draw(*cursor);
	debug.RenderCollisionBoxes();
	endDraw();

}



std::string MainMenu::GetTypedName() {
	std::string name = playerNameTyped;
	if (name.size() <= 0)
	{
		return "DefaultName";
	}
	return name;
}