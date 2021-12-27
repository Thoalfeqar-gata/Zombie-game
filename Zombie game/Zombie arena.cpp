#include "Zombie arena.h"
Zombie* createHoard(int numZombies, sf::IntRect arena)
{
	Zombie* zombies = new Zombie[numZombies];
	int maxY = arena.height - 20;
	int minY = arena.top + 20;
	int maxX = arena.width - 20;
	int minX = arena.left + 20;

	for (int i = 0; i < numZombies; i++)
	{
		srand(time(0) * i);
		int side = rand() % 4;
		float x, y;
		switch (side)
		{
		case 0:
			//left
			x = minX;
			y = rand() % maxY + minY;
			break;

		case 1:
			//right
			x = maxX;
			y = rand() % maxY + minY;
			break;

		case 2:
			//top
			y = minY;
			x = rand() % maxX + minX;
			break;

		case 3:
			//bottom
			y = maxY;
			x = rand() % maxX + minX;
			break;
		}

		srand(time(0) * i * 3);
		int type = rand() % 3;
		zombies[i].spawn(x, y, type, i);
	}
	return zombies;
}

int createBackground(sf::VertexArray& rVA, sf::IntRect arena)
{
	const int TILE_SIZE = 50;
	const int TILE_TYPES = 3; // 0 TO 3 = 4
	const int VERTS_IN_QUAD = 4;
	int worldWidth = arena.width / TILE_SIZE;
	int worldHeight = arena.height / TILE_SIZE;
	rVA.setPrimitiveType(sf::Quads);
	rVA.resize(VERTS_IN_QUAD * worldWidth * worldHeight);

	int currentIndex = 0;

	for (int w = 0; w < worldWidth; w++)
	{
		for (int h = 0; h < worldHeight; h++)
		{
			rVA[currentIndex + 0].position = sf::Vector2f(0 + TILE_SIZE * w, 0 + TILE_SIZE * h);
			rVA[currentIndex + 1].position = sf::Vector2f(0 + TILE_SIZE * w + TILE_SIZE, TILE_SIZE * h);
			rVA[currentIndex + 2].position = sf::Vector2f(0 + TILE_SIZE * w + TILE_SIZE, TILE_SIZE * h + TILE_SIZE);
			rVA[currentIndex + 3].position = sf::Vector2f(0 + TILE_SIZE * w, 0 + TILE_SIZE * h + TILE_SIZE);


			if (w == 0 || w == worldWidth - 1 || h == 0 || h == worldHeight - 1)
			{
				rVA[currentIndex + 0].texCoords = sf::Vector2f(0, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentIndex + 1].texCoords = sf::Vector2f(0 + TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentIndex + 2].texCoords = sf::Vector2f(0 + TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE + TILE_SIZE);
				rVA[currentIndex + 3].texCoords = sf::Vector2f(0, 0 + TILE_TYPES * TILE_SIZE + TILE_SIZE);
			}
			else
			{
				srand(time(0) + w * h - h);
				int mOrg = rand() % TILE_TYPES;
				int verticalOffset = mOrg * TILE_SIZE;

				rVA[currentIndex + 0].texCoords = sf::Vector2f(0, 0 + verticalOffset);
				rVA[currentIndex + 1].texCoords = sf::Vector2f(0 + TILE_SIZE, 0 + verticalOffset);
				rVA[currentIndex + 2].texCoords = sf::Vector2f(0 + TILE_SIZE, 0 + verticalOffset + TILE_SIZE);
				rVA[currentIndex + 3].texCoords = sf::Vector2f(0, 0 + verticalOffset + TILE_SIZE);
			}

			currentIndex += VERTS_IN_QUAD;
		}
	}
	return TILE_SIZE;
}