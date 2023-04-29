#include "UIManager.h"
UIManager::UIManager()
{
	font = new sf::Font();
	if (font->loadFromFile("font/arial.ttf"))
	{
	}
	pauseMenuOn = false;
}

UIManager::~UIManager()
{
	delete font;
}

void UIManager::SetInput(Input* in)
{
	for (int i = 0; i < UIElements.size(); i++)
	{
		UIElements[i].setInput(in);
	}
	for (int i = 0; i < PauseMenu.size(); i++)
	{
		PauseMenu[i].setInput(in);
	}
}

void UIManager::Render(sf::RenderWindow* window)
{
	for (int i = 0; i < UIElements.size(); i++)
	{
		window->draw(UIElements[i]);
		window->draw(UIElements[i].text);
	}
	if (pauseMenuOn)
	{
		for (int i = 0; i < PauseMenu.size(); i++)
		{
			window->draw(PauseMenu[i]);
			window->draw(PauseMenu[i].text);
		}
	}


}

void UIManager::Update()
{
	for (int i = 0; i < UIElements.size(); i++)
	{
		UIElements[i].update();
	}
	for (int i = 0; i < PauseMenu.size(); i++)
	{
		PauseMenu[i].update();
	}
}

void UIManager::ShowPauseMenu(sf::RenderWindow* win)
{
	sf::Vector2f newPos = win->getView().getCenter();

	for (int i = 1; i < PauseMenu.size(); i++)
	{
		sf::Vector2f difference = PauseMenu[i].getPosition() - PauseMenu[0].getPosition();
		PauseMenu[i].setPosition(newPos + difference);//PauseMenu[i].getPosition()  newPos);
		PauseMenu[i].text.setPosition(newPos + difference);
	}
	PauseMenu[0].setPosition(newPos);
	pauseMenuOn = !pauseMenuOn;
}

void UIManager::NormaliseBoxes(std::vector<UIElement>& ui)
{
	sf::Vector2f size = sf::Vector2f(0,0);
	for (int i = 0; i < ui.size(); i++)
	{
		if (size.x <= ui[i].getSize().x)
		{
			size = sf::Vector2f(ui[i].getSize().x, size.y);
		}
		if (size.y <= ui[i].getSize().y)
		{
			size = sf::Vector2f(size.x, ui[i].getSize().y);
		}
	}

	for (int i = 0; i < ui.size(); i++)
	{
		ui[i].Resize(size);
	}
}

void UIManager::CenterAllText(std::vector<UIElement>& ui)
{
	for (int i = 0; i < ui.size(); i++)
	{
		ui[i].CenterText();
	}
}
