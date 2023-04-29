#include "UIElement.h"

UIElement::UIElement(sf::Font* font)
{
	setFillColor(sf::Color::Red);
	setSize(sf::Vector2f(200, 24));
	setOutlineColor(sf::Color::Black);
	setOutlineThickness(1);
	setCollider(true);
	setCollisionBox(0, 0, 100, 100);
	//setPosition(222,222);
	setPosition(50, 50);
	//setOrigin(222,22);


	text.setFont(*font);
	text.setCharacterSize(24);
	text.setString("A");
	text.setFillColor(sf::Color::White);
	text.setPosition(getPosition().x + leftPadding, getPosition().y + topPadding);
}

UIElement::UIElement()
{
}

UIElement::~UIElement()
{
}

void UIElement::Setup(bool colOn, std::string text, sf::Vector2f position, sf::Color boxColor, sf::Color lineColor, float lineThickness)
{
	setAlive(colOn);
	setPosition(position); setString(text);
	setFillColor(boxColor); setOutlineColor(lineColor); setOutlineThickness(lineThickness);
	setCollider(colOn);
	orginalColor = boxColor;
}

void UIElement::Setup(bool colOn, std::string text, sf::Vector2f position, sf::Vector2f boxSize, sf::Color boxColor, sf::Color lineColor, float lineThickness)
{
	setAlive(colOn);
	setPosition(position); 
	setString(text); 
	Resize(boxSize);
	setFillColor(boxColor); setOutlineColor(lineColor); setOutlineThickness(lineThickness);
	setCollider(colOn);
	orginalColor = boxColor;
}

void UIElement::Setup(bool colOn, std::string text, sf::Vector2f position, sf::Texture* boxTexture, sf::Color lineColor, float lineThickness)
{
	setAlive(colOn);
	setPosition(position); setString(text);
	setTexture(boxTexture); setOutlineColor(lineColor); setOutlineThickness(lineThickness);
	setCollider(colOn);
}

void UIElement::Setup(bool colOn, std::string text, sf::Vector2f position, sf::Vector2f boxSize, sf::Texture* boxTexture, sf::Color lineColor, float lineThickness)
{
	setAlive(colOn);
	setPosition(position); setString(text); Resize(boxSize);
	setTexture(boxTexture); setOutlineColor(lineColor); setOutlineThickness(lineThickness);
	setCollider(colOn);
}

void UIElement::Resize(sf::Vector2f size)
{
	setSize(size);
	setCollisionBox(0, 0, size.x, size.y);
}

void UIElement::Resize(float x, float y)
{
	setSize(sf::Vector2f(x,y));
	setCollisionBox(0, 0, x, y);
}

void UIElement::CenterText()
{
	text.setOrigin(0, 0);
	std::string str = text.getString();
	sf::Vector2f endPos = text.findCharacterPos(str.size());
	sf::Vector2f size = ((endPos - text.getPosition()));
	size.y += text.getCharacterSize();
	//text.setPosition(0, 0);
	//text.setOrigin(size/2.f + text.getPosition());
	text.setOrigin(size / 2.f);
	text.setPosition((getSize() / 2.f + getPosition()));// -getPosition()));
	text.setPosition(text.getPosition().x, text.getPosition().y + yPush);//- (text.getCharacterSize() / 2));
}



void UIElement::setString(std::string str)
{
	text.setOrigin(0, 0); 	text.setPosition(getPosition().x + leftPadding, getPosition().y + topPadding);
	text.setString(str);
	sf::Vector2f endPos = text.findCharacterPos(str.size());	
	Resize(sf::Vector2f((endPos.x - getPosition().x) + rightPadding, ((endPos.y + text.getCharacterSize()) - getPosition().y) + bottomPadding));
	text.setPosition(getPosition().x + leftPadding, getPosition().y + topPadding);
	
}

void UIElement::setPadding(float LeftPadding, float RightPadding, float TopPadding, float BottomPadding)
{
	leftPadding = LeftPadding; rightPadding = RightPadding; topPadding = TopPadding; bottomPadding = BottomPadding;
}

void UIElement::update()
{
	setFillColor(orginalColor);
}

void UIElement::HoverOver()
{
	int r, g, b;
	r = getFillColor().r; g = getFillColor().g; b = getFillColor().b;
	if (r > 50) { r -= 50; }
	if (g > 50) { g -= 50; }
	if (b > 50) { b -= 50; }
	sf::Color highlightColor = sf::Color(r, g, b, getFillColor().a);
	//highlightColor += getFillColor();
	setFillColor(highlightColor);
	//setFillColor(sf::Color::Black);
}


void UIElement::collisionResponse(GameObject*)
{
	HoverOver();
}

bool UIElement::collisionResponseClick()
{
	if (input->isLeftMousePressed())
	{
		return true;
	}
	return false;
}
