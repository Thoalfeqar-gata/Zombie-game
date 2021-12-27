#include "Bullet.h"
Bullet::Bullet()
{
	m_Shape.setFillColor(sf::Color::Red);
	m_InFlight = false;
	m_BulletSpeed = 1000;
	m_Shape.setSize(sf::Vector2f(5, 5));
}

void Bullet::stop()
{
	m_InFlight = false;
}

bool Bullet::inFlight()
{
	return m_InFlight;
}

void Bullet::shoot(float startX, float startY, float targetX, float targetY)
{
	m_InFlight = true;
	m_Position.x = startX;
	m_Position.y = startY;

	float gradient = (startX - targetX) / (startY - targetY);
	if (gradient < 0)
	{
		gradient *= -1;
	}

	float ratioXY = m_BulletSpeed / (1 + gradient);
	m_BulletDistanceY = ratioXY;
	m_BulletDistanceX = ratioXY * gradient;

	if (targetX < startX)
	{
		m_BulletDistanceX *= -1;
	}

	if (targetY < startY)
	{
		m_BulletDistanceY *= -1;
	}

	float range = 1000;
	m_MinX = startX - range;
	m_MaxX = startX + range;
	m_MinY = startY - range;
	m_MaxY = startY + range;

	m_Shape.setPosition(m_Position);
}

void Bullet::update(float elapsedTime)
{
	m_Position.x += m_BulletDistanceX * elapsedTime;
	m_Position.y += m_BulletDistanceY * elapsedTime;

	m_Shape.setPosition(m_Position);

	if (m_Position.x < m_MinX || m_Position.x > m_MaxX ||
		m_Position.y < m_MinY || m_Position.y > m_MaxY)
	{
		m_InFlight = false;
	}
}

sf::FloatRect Bullet::getPosition()
{
	return m_Shape.getGlobalBounds();
}

sf::RectangleShape Bullet::getShape()
{
	return m_Shape;
}
