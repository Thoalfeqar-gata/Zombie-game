#include <iostream>
#include <sstream>
#include <fstream>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "Zombie arena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickups.h"
#include "Player.h"


int main()
{
	TextureHolder holder;
	enum class State
	{
		PAUSED,
		LEVELING_UP,
		GAME_OVER,
		PLAYING
	};

	State state = State::GAME_OVER;

	sf::Vector2f resolution;
	resolution.x = sf::VideoMode::getDesktopMode().width;
	resolution.y = sf::VideoMode::getDesktopMode().height;

	sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Zombies", sf::Style::Fullscreen);
	sf::View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));
	window.setMouseCursorVisible(false);
	sf::Sprite crossHairSprite;
	crossHairSprite.setOrigin(25, 25);
	sf::Texture crossHairTexture = TextureHolder::getTexture(R"(Media\graphics\crosshair.png)");
	crossHairSprite.setTexture(crossHairTexture);
	//timing
	sf::Clock clock;
	//how long has the PLAYING state been active
	sf::Time gameTotalTime;

	//mouse coordinates relative to the world and the screen
	sf::Vector2f mouseWorldPosition;
	sf::Vector2i mouseScreenPosition;

	Player player;

	sf::IntRect arena;

	sf::VertexArray background;
	sf::Texture backgroundTexture = TextureHolder::getTexture(R"(Media\graphics\background_sheet.png)");

	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 240;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 2;
	sf::Time lastTimeShot = sf::Time::Zero;


	Pickups healthPickup(1);
	Pickups ammoPickup(2);


	int score = 0;
	int highScore = 0;

	sf::Sprite gameOverBackgroundSprite;
	sf::Texture gameOverBackgroundTexture = TextureHolder::getTexture(R"(Media\graphics\background.png)");
	gameOverBackgroundSprite.setTexture(gameOverBackgroundTexture);
	gameOverBackgroundSprite.setPosition(0, 0);

	sf::View hudView(sf::FloatRect(0, 0, 1920, 1080));

	std::ifstream inputFile(R"(Media\gamedata\score.txt)");
	if (inputFile.is_open())
	{
		inputFile >> highScore;
		inputFile.close();
	}

	sf::Sprite ammoIconSprite;
	sf::Texture ammoIconTexture = TextureHolder::getTexture(R"(Media\graphics\ammo_icon.png)");
	ammoIconSprite.setTexture(ammoIconTexture);
	ammoIconSprite.setPosition(20, 980);
	sf::Font font;
	font.loadFromFile(R"(Media\fonts\zombiecontrol.ttf)");

	//paused
	sf::Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(sf::Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter\nto continue");


	// Game Over
	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	// LEVELING up
	sf::Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(sf::Color::White);
	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream <<
		"1- Increased rate of fire" <<
		"\n2- Increased clip size(next reload)" <<
		"\n3- Increased max health" <<
		"\n4- Increased run speed" <<
		"\n5- More and better health pickups" <<
		"\n6- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	// Ammo
	sf::Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(sf::Color::White);
	ammoText.setPosition(200, 980);

	// Score
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20, 0);

	// High Score
	sf::Text highScoreText;
	highScoreText.setFont(font);
	highScoreText.setCharacterSize(55);
	highScoreText.setFillColor(sf::Color::White);
	highScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "High Score:" << highScore;
	highScoreText.setString(s.str());

	// Zombies remaining
	sf::Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);
	zombiesRemainingText.setFillColor(sf::Color::White);
	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies: 100");

	// Wave number
	int wave = 0;
	sf::Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(sf::Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// Health bar
	sf::RectangleShape healthBar;
	healthBar.setFillColor(sf::Color::Red);
	healthBar.setPosition(450, 980);

	int framesSinceLastHudUpdate = 0;
	//update the hud only after 1000 frames
	int	fpsMeasurementFramInterval = 1000;

	sf::SoundBuffer hitBuffer;
	sf::SoundBuffer reloadBuffer;
	sf::SoundBuffer failedToReloadBuffer;
	sf::SoundBuffer shootBuffer;
	sf::SoundBuffer splatBuffer;
	sf::SoundBuffer pickupBuffer;
	sf::SoundBuffer powerupBuffer;
	hitBuffer.loadFromFile(R"(Media\sound\hit.wav)");
	reloadBuffer.loadFromFile(R"(Media\sound\reload.wav)");
	failedToReloadBuffer.loadFromFile(R"(Media\sound\reload_failed.wav)");
	shootBuffer.loadFromFile(R"(Media\sound\shoot.wav)");
	splatBuffer.loadFromFile(R"(Media\sound\splat.wav)");
	pickupBuffer.loadFromFile(R"(Media\sound\pickup.wav)");
	powerupBuffer.loadFromFile(R"(Media\sound\powerup.wav)");

	sf::Sound hitSound;
	sf::Sound reloadSound;
	sf::Sound failedReloadSound;
	sf::Sound shootSound;
	sf::Sound splatSound;
	sf::Sound pickupSound;
	sf::Sound powerupSound;
	hitSound.setBuffer(hitBuffer);
	reloadSound.setBuffer(reloadBuffer);
	failedReloadSound.setBuffer(failedToReloadBuffer);
	shootSound.setBuffer(shootBuffer);
	splatSound.setBuffer(splatBuffer);
	pickupSound.setBuffer(pickupBuffer);
	powerupSound.setBuffer(powerupBuffer);


	while (window.isOpen())
	{
		//........................//
		//the input handling stuff//
		//........................//
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Return && state == State::PLAYING)
				{
					state = State::PAUSED;
				}

				else if (event.key.code == sf::Keyboard::Return && state == State::PAUSED)
				{
					state = State::PLAYING;
					//pausing is considered to be a long frame, so restart it.
					clock.restart();
				}

				else if (event.key.code == sf::Keyboard::Return && state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;
					wave = 0;
					score = 0;

					currentBullet = 0;
					bulletsSpare = 60;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;
					player.resetPlayerStats();
				}

				if (state == State::PLAYING)
				{
					if (event.key.code == sf::Keyboard::R)
					{
						if (bulletsSpare >= clipSize)
						{
							reloadSound.play();
							bulletsSpare -= clipSize - bulletsInClip;
							bulletsInClip = clipSize;
						}
						else if (bulletsSpare > 0)
						{
							reloadSound.play();
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
						}
						else
						{
							failedReloadSound.play();
						}
					}
				}
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		if (state == State::PLAYING)
		{
			// Handle the pressing and releasing of the WASD keys
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{

				if (gameTotalTime.asMilliseconds() - lastTimeShot.asMilliseconds()
					> (1000 / fireRate) && bulletsInClip > 0)
				{
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
					currentBullet++;
					if (currentBullet > 99)
					{
						currentBullet = 0;
					}
					lastTimeShot = gameTotalTime;
					bulletsInClip--;
					shootSound.play();
				}
			}
		}

		if (state == State::LEVELING_UP)
		{
			if (event.key.code == sf::Keyboard::Num1)
			{
				fireRate++;
				state = State::PLAYING;
			}

			if (event.key.code == sf::Keyboard::Num2)
			{
				clipSize += clipSize;
				state = State::PLAYING;
			}

			if (event.key.code == sf::Keyboard::Num3)
			{
				player.increaseMaxHealth();
				state = State::PLAYING;
			}

			if (event.key.code == sf::Keyboard::Num4)
			{
				player.upgradeSpeed();
				state = State::PLAYING;
			}

			if (event.key.code == sf::Keyboard::Num5)
			{
				healthPickup.upgrade();
				state = State::PLAYING;
			}

			if (event.key.code == sf::Keyboard::Num6)
			{
				ammoPickup.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING)
			{
				wave++;
				arena = sf::IntRect(0, 0, 500 * wave, 500 * wave);
				int tileSize = createBackground(background, arena);
				player.spawn(arena, resolution, tileSize);

				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

				numZombies = 5 * wave;
				delete[] zombies;
				zombies = createHoard(numZombies, arena);
				numZombiesAlive = numZombies;
				//restart the clock so there isn't any frame jump
				powerupSound.play();
				clock.restart();
			}

		}


		//.......................//
		//updating the game stuff//
		//.......................//
		if (state == State::PLAYING)
		{
			sf::Time dt = clock.restart();
			gameTotalTime += dt;

			float dtAsSeconds = dt.asSeconds();

			mouseScreenPosition = sf::Mouse::getPosition();
			mouseWorldPosition = window.mapPixelToCoords(sf::Mouse::getPosition(), mainView);
			player.update(dtAsSeconds, sf::Mouse::getPosition());

			crossHairSprite.setPosition(mouseWorldPosition);
			sf::Vector2f playerPosition(player.getCenter());

			for (int i = 0; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
					zombies[i].update(dtAsSeconds, playerPosition);
			}

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].inFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			//collision
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numZombies; j++)
				{
					if (bullets[i].inFlight() && zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{
							bullets[i].stop();
							if (zombies[j].hit())
							{
								splatSound.play();
								score += 10;
								numZombiesAlive--;
								if (score >= highScore)
								{
									highScore = score;
									std::ofstream outputFile(R"(Media\gamedata\score.txt)");
									outputFile << highScore;
									outputFile.close();
								}

								if (numZombiesAlive == 0)
								{
									state = State::LEVELING_UP;
								}
							}
						}
					}
				}
			}

			for (int i = 0; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
				{
					if (player.getPosition().intersects(zombies[i].getPosition()))
					{
						if (player.hit(gameTotalTime))
						{
							hitSound.play();
						}

						if (player.getHealth() <= 0)
						{
							state = State::GAME_OVER;
						}
					}
				}
			}

			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare += ammoPickup.gotIt();
				pickupSound.play();
			}

			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealth(healthPickup.gotIt());
				reloadSound.play();
			}

			healthBar.setSize(sf::Vector2f(player.getHealth() * 3, 50));
			framesSinceLastHudUpdate++;

			if (framesSinceLastHudUpdate > fpsMeasurementFramInterval)
			{
				// Update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				// Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score: " << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "High Score:" << highScore;
				highScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());
				framesSinceLastHudUpdate = 0;
			}

			mainView.setCenter(player.getCenter());
		}

		//..............//
		//draw the scene//
		//..............//
		if (state == State::PLAYING)
		{
			window.clear();

			window.setView(mainView);
			window.draw(background, &backgroundTexture);

			for (int i = 0; i < numZombies; i++)
			{
				window.draw(zombies[i].getSprite());
			}

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].inFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			if (ammoPickup.isSpawned())
			{
				window.draw(ammoPickup.getSprite());
			}

			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}

			window.draw(crossHairSprite);
			window.draw(player.getSprite());


			window.setView(hudView);
			// Draw all the HUD elements
			window.draw(ammoIconSprite);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(highScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELING_UP)
		{
			window.setView(hudView);
			window.draw(gameOverBackgroundSprite);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED)
		{
			window.setView(hudView);
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER)
		{
			window.setView(hudView);
			window.draw(gameOverBackgroundSprite);
			window.draw(gameOverText);

			window.draw(scoreText);
			window.draw(highScoreText);
		}

		window.display();
	}
	return 0;
}