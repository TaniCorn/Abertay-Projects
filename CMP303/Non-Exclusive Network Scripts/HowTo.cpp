#include "HowTo.h"

HowTo::HowTo(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	////////For some reason debug, must be up here 
	cursor = new Cursor(hwnd, in);
	background = Background(hwnd, in);
	debug = CollisionDebugBox(hwnd, in);
	howToImage = new sf::Texture();
	if (!howToImage->loadFromFile("gfx/howto.png"))
	{
		std::cout << "WHY";
	}
	background.overlay.setTexture(howToImage);
	background.overlay.setSize(sf::Vector2f(2048, 2048));

#pragma region SETUP_UI_ELEMENTS
	UI.UIElements.push_back(UIElement(UI.font));
	UI.UIElements[0].setPadding(0, 50, 0, 20);	//UI.UIElements[0].setInput(in);
	UI.UIElements[0].Setup(true, "Back", sf::Vector2f(510, 510), sf::Vector2f(100, 60), sf::Color::Magenta, sf::Color::Black, 1.f);
	UI.SetInput(in);
	UI.NormaliseBoxes(UI.UIElements); UI.CenterAllText(UI.UIElements);
#pragma endregion

#pragma region SETUP_COLLISION_MANAGER
	collisionManager.cursor = cursor;
	collisionManager.clickableUIElements.push_back(&UI.UIElements[0]);
#pragma endregion


}
void HowTo::init() {

}
HowTo::~HowTo()
{
	delete cursor;
	//delete howToImage;
	delete background.backgroundTexture;
	delete background.overlayTexture;
}

void HowTo::handleInput(float dt)
{
	background.handleInput(dt);
}

void HowTo::update(float dt)
{

	//debug.update(dt);
	background.update(dt);
	cursor->update(dt);
	UI.Update();

	if (background.UpdateUIElements)
	{
		background.overlay.setPosition(window->getView().getCenter() - (window->getView().getSize() / 2.f));

	}
#pragma region COLLISIONCHECKS

	uiResponse = collisionManager.UICollision();
	switch (uiResponse)
	{
	case(0):
		gameState->setCurrentState(GameState::State::MENU);
		break;
	default:
		break;
	}
#pragma endregion
}

void HowTo::render()
{
	beginDraw();
	window->draw(background.overlay);
	UI.Render(window);
	window->draw(*cursor);
	endDraw();

}
