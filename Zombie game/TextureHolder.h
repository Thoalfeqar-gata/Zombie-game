#pragma once
#ifndef TEXTURE_HOLDER_H
#define TEXTRUE_HOLDER_H
#include "SFML/Graphics.hpp"
#include <map>

class TextureHolder
{
private:
	//where we will hold the actual textures
	std::map<std::string, sf::Texture> m_Textures;

	// a static pointer to the only instance of this class
	static TextureHolder* m_s_Instance;

public:
	TextureHolder();
	static sf::Texture& getTexture(const std::string& filename);
};
#endif

