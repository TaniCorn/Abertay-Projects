// Main
// Entry point of the application.
// Handles window creation and window events.
// Contains the game loop, updates input class and contains the level objects.
// @author Paul Robertson

#include <iostream>
#include "HowTo.h"
#include "Level.h"
#include "MainMenu.h"
#include "Lobby.h"
#include "Framework/AudioManager.h"
#include "Framework/GameState.h"

void windowProcess(sf::RenderWindow* window, Input* in)
{
	// Handle window events.
	sf::Event event;
	while (window->pollEvent(event))
	{
		
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::Resized:
			window->setView(sf::View(sf::FloatRect(0.f, 0.f, (float)event.size.width, (float)event.size.height)));
			
			break;
		case sf::Event::KeyPressed:
			// update input class
			in->setKeyDown(event.key.code);
			break;
		case sf::Event::KeyReleased:
			//update input class
			in->setKeyUp(event.key.code);
			break;
		case sf::Event::MouseMoved:
			//update input class
			in->setMousePosition(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//update input class
				in->setLeftMouse(Input::MouseState::DOWN);
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				in->setRightMouse(Input::MouseState::DOWN);
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//update input class
				in->setLeftMouse(Input::MouseState::UP);
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				in->setRightMouse(Input::MouseState::UP);
			}
			break;
		default:
			// don't handle other events
			break;
		}
	}
	//std::cout << in->getMouseX() << std::endl;
	///std::cout << " Y :" << std::endl;
	//std::cout << in->getMouseY() << std::endl;
	//When resizing the window, it stretches or shrinks the objects, thus our cursor can exit the screen and the cursor object is only half way. We need to stop the stretching.
}

int main()
{
	srand(time(0));
	//Create the window
	sf::RenderWindow window(sf::VideoMode(1500, 800), "Behind the Undead");

	// Initialise input and level objects.
	AudioManager audioManager;
	Input input;
	GameState gameState; gameState.setCurrentState(GameState::State::MENU);
	MainMenu mainMenu(&window, &input, &gameState, &audioManager);
	Level* level = new Level(&window, &input, &gameState, &audioManager);
	HowTo howTo(&window, &input, &gameState, &audioManager);
	Lobby lobby(&window, &input, &gameState, &audioManager);

	bool createLevel = true;
	// Initialise objects for delta time
	sf::Clock clock;
	float deltaTime;

	// Game Loop
	while (window.isOpen())
	{
		if (gameState.getCurrentState() == GameState::State::PAUSE)
		{
			level->clean();
			delete level;
			gameState.setCurrentState(GameState::State::MENU);
			level = new Level(&window, &input, &gameState, &audioManager);
		}
		//Process window events
		windowProcess(&window, &input);

		// Calculate delta time. How much time has passed 
		// since it was last calculated (in seconds) and restart the clock.
		deltaTime = clock.restart().asSeconds();

		switch (gameState.getCurrentState())
		{
		case(GameState::State::MENU):
			if (!mainMenu.initialised)
				mainMenu.init();

			mainMenu.handleInput(deltaTime);
			mainMenu.update(deltaTime);
			mainMenu.render();
			break;		
		case(GameState::State::LOBBY):
			if (!lobby.initialised)
				lobby.init();
			lobby.handleInput(deltaTime);
			lobby.update(deltaTime);
			lobby.render();
			break;
		case(GameState::State::HOWTO):
			if (!howTo.initialised)
				howTo.init();
			howTo.handleInput(deltaTime);
			howTo.update(deltaTime);
			howTo.render();
			break;
		case(GameState::State::LEVEL):
			if (!level->initialised)
				level->init();
			//audioManager.stopAllMusic();
			level->handleInput(deltaTime);
			level->update(deltaTime);
			level->render();
			break;
		case(GameState::State::EXIT):
			window.close();
			break;
		default:
			break;
		}
		// Call standard game loop functions (input, update and render)




		// Update input class, handle pressed keys
		// Must be done last.
		input.update();
	}
}
