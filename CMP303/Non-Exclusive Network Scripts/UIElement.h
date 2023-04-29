#pragma once   
#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "framework/GameObject.h"
class UIElement :
    public GameObject
{
public:
    #pragma region CONSTRUCTORS/DESTRUCTORS
    UIElement(sf::Font*);
    UIElement();
    ~UIElement();
    /// <summary>
    /// Sets up a box with text and position. Can specify box color, line color and thickness.
    /// <para>The size of the box is predefined with padding size and text size. </para>
    /// </summary>
    void Setup(bool collision, std::string text, sf::Vector2f position, sf::Color boxColor = sf::Color::Transparent, sf::Color lineColor = sf::Color::Transparent, float lineThickness = 0);
    /// <summary>
    /// Sets up a box with text, position, size. Can specify box color, line color and thickness.
    /// <para>NOTE: If using this option, it is best to manually set the text position.</para>
    /// </summary>
    void Setup(bool collision, std::string text, sf::Vector2f position, sf::Vector2f boxSize, sf::Color boxColor = sf::Color::Transparent, sf::Color lineColor = sf::Color::Transparent, float lineThickness = 0);
    /// <summary>
    /// Sets up a box with text, position and texture. Can specify line color and thickness.
    /// <para>The size of the box is predefined with padding size and text size. </para>
    /// </summary>
    void Setup(bool collision, std::string text, sf::Vector2f position, sf::Texture* boxTexture, sf::Color lineColor = sf::Color::Transparent, float lineThickness = 0);
    /// <summary>
    /// Sets up a box with text, position, size and texture. Can specify line color and thickness.
    /// <para>NOTE: If using this option, it is best to manually set the text position.</para>
    /// </summary>
    void Setup(bool collision, std::string text, sf::Vector2f position, sf::Vector2f boxSize, sf::Texture* boxTexture, sf::Color lineColor = sf::Color::Transparent, float lineThickness = 0);
#pragma endregion

    void update();

    #pragma region GENERAL_FUNCTIONS_AND_COLLISIONS
    /// <summary>
    /// Manually resizes the box containing text.
    /// </summary>
    void Resize(sf::Vector2f size);
    /// <summary>
    /// Manually resizes the box containing text.
    /// </summary>
    void Resize(float x, float y);
    /// <summary>
    /// Centers the text on the box, no regard for resizing the box.
    /// </summary>
    void CenterText();
    /// <summary>
    /// Sets the string of the text and resizes the box automatically, with padding.
    /// </summary>
    void setString(std::string text);
    /// <summary>
    /// Sets the padding
    /// </summary>
    void setPadding(float LeftPadding, float RightPadding, float TopPadding, float BottomPadding);


    void HoverOver();
    virtual void collisionResponse(GameObject*) override;
    bool collisionResponseClick();
#pragma endregion

    float leftPadding = 10; float topPadding = 10;
    float rightPadding = 10; float bottomPadding = 10;

    float yPush = -2;//yPush is the amount of pushing the text gets on the y direction when centered. In case it doesn't look so centered or you want it to be closer to the bottom or the top. Manual setting.
    sf::Text text; //Text MUST be handled as well as the UIElement. E.g Render UIElement and UIElement.text
    sf::Color orginalColor;
};

#endif // !UIELEMENT_H