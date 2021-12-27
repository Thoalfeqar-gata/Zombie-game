#pragma once
#include "SFML/Graphics.hpp"
#include "Zombie.h"
Zombie* createHoard(int numZombies, sf::IntRect arena);
int createBackground(sf::VertexArray& rVA, sf::IntRect arena);