#ifndef UIMANAGER_H
#define UIMANAGER_H


#include "UIElement.h"
/// <summary>
/// This class is a bit of a mess, it contains the main menu ui and pause menu ui, IT DOES NOT manage all ui in the game whatsoever.
/// </summary>
class UIManager
{
public:
    UIManager();
    ~UIManager();
    void SetInput(Input* in);
    void Render(sf::RenderWindow*s);
    void Update();
    void ShowPauseMenu(sf::RenderWindow*);
    bool pauseMenuOn;
    void NormaliseBoxes(std::vector<UIElement>&);
    void CenterAllText(std::vector<UIElement>&);
	sf::Font* font;

    std::vector<UIElement> UIElements;
    std::vector<UIElement> PauseMenu;
};

#endif // !UIMANAGER_H
