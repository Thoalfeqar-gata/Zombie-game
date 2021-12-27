#include "Pickups.h"
#include <iostream>
#include "TextureHolder.h"
Pickups::Pickups(int type)
{
	m_Type = type;

	if (m_Type == 1)
	{
		m_Sprite = sf::Sprite(TextureHolder::getTexture(R"(Media\graphics\health_pickup.png)"));
		m_Value = HEALTH_START_VALUE;
	}
	else
	{
		m_Sprite = sf::Sprite(TextureHolder::getTexture(R"(Media\graphics\ammo_pickup.png)"));
		m_Value = AMMO_START_VALUE;
	}

	m_SecondsToLive = START_SECONDS_TO_LIVE;
	m_SecondsToWait = START_WAIT_TIME;
}

void Pickups::setArena(sf::IntRect arena)
{
	m_Arena.left = arena.left + 50;
	m_Arena.top = arena.top + 50;
	m_Arena.width = arena.width - 50;
	m_Arena.height = arena.height - 50;
	spawn();
}


void Pickups::spawn()
{
	srand(time(NULL) * m_Type + m_Type);
	int x = rand() % m_Arena.width;
	srand(time(NULL) * m_Type + m_Type);
	int y = rand() % m_Arena.height;
	m_SecondsSinceSpawn = 0;
	m_Spawned = true;
	m_Sprite.setPosition(x, y);
}

sf::FloatRect Pickups::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

sf::Sprite Pickups::getSprite()
{
	return m_Sprite;
}

void Pickups::update(float elapsedTime)
{
	if (m_Spawned)
	{
		m_SecondsSinceSpawn += elapsedTime;
	}
	else
	{
		m_SecondsSinceDeSpawn += elapsedTime;
	}

	if (m_SecondsSinceSpawn > m_SecondsToLive && m_Spawned)
	{
		m_Spawned = false;
		m_SecondsSinceDeSpawn = 0;
	}

	if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned)
	{
		spawn();
	}
}

bool Pickups::isSpawned()
{
	return m_Spawned;
}

int Pickups::gotIt()
{
	m_SecondsSinceDeSpawn = 0;
	m_Spawned = false;
	return m_Value;
}

void Pickups::upgrade()
{
	if (m_Type == 1)
	{
		m_Value += HEALTH_START_VALUE * 0.5;
	}
	else
	{
		m_Value += AMMO_START_VALUE * 0.5;
	}

	m_SecondsToLive += START_SECONDS_TO_LIVE / 10;
	m_SecondsToWait -= START_WAIT_TIME / 10;
}
