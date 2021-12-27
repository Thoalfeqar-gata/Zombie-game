#pragma once
#include "SFML/Graphics.hpp"
class Bullet
{
private:
	sf::Vector2f m_Position;
	sf::RectangleShape m_Shape;
	bool m_InFlight;
	float m_BulletSpeed;
	float m_BulletDistanceX;
	float m_BulletDistanceY;
	float m_MinX;
	float m_MaxX;
	float m_MinY;
	float m_MaxY;

public:
	Bullet();
	void stop();
	bool inFlight();
	void shoot(float startX, float startY, float targetX, float targetY);
	void update(float elapsedTime);
	sf::FloatRect getPosition();
	sf::RectangleShape getShape();
};

