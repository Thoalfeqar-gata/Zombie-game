#pragma once
#include "SFML/Graphics.hpp"
class Zombie
{
	const float BLOATER_SPEED = 40;
	const float CHASER_SPEED = 80;
	const float CRAWLER_SPEED = 20;

	const float BLOATER_HEALTH = 5;
	const float CHASER_HEALTH = 1;
	const float CRAWLER_HEALTH = 3;

	const int MAX_VARIANCE = 30;
	const int OFFSET = 101 - MAX_VARIANCE;

	sf::Vector2f m_Position;
	sf::Sprite m_Sprite;
	float m_Speed;
	float m_Health;
	bool m_Alive;
public:

	bool hit();
	bool isAlive();
	void spawn(float startX, float startY, int type, int seed);
	sf::FloatRect getPosition();
	sf::Sprite getSprite();
	void update(float elapsedTime, sf::Vector2f playerLocation);
};

