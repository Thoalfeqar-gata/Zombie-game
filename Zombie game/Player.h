#pragma once
#include "SFML/Graphics.hpp"
class Player
{
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	sf::Vector2f m_Position;
	sf::Sprite m_Sprite;
	sf::Texture m_Texture;
	//Actual Screen resolution
	sf::Vector2f m_Resolution;
	//arena size
	sf::IntRect m_Arena;
	int m_TileSize;
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;
	int m_Health;
	int m_MaxHealth;
	sf::Time m_LastHit;
	float m_Speed;



public:

	Player();
	void spawn(sf::IntRect arena, sf::Vector2f resolution, int tileSize);
	void resetPlayerStats();

	//handle the player being hit
	bool hit(sf::Time timeHit);
	sf::Time getLastHitTime();
	sf::FloatRect getPosition();
	sf::Vector2f getCenter();
	float getRotation();
	sf::Sprite getSprite();

	//movement
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();

	void update(float elapsedTime, sf::Vector2i mousePosition);
	void upgradeSpeed();
	void increaseMaxHealth();
	void increaseHealth(int amount);
	int getHealth();
	sf::Vector2f getResolution();
};

