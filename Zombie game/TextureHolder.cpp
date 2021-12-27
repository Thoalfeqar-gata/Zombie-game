#include "TextureHolder.h"
#include <assert.h>
TextureHolder* TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder()
{
	//if the static pointer has already been assigned to, throw an error
	assert(m_s_Instance == nullptr);
	//assign it to 'this' otherwise. As 'this' is the pointer to first (and only)
	//instance of this class
	m_s_Instance = this;
}

sf::Texture& TextureHolder::getTexture(const std::string& filename)
{
	//get the map using the static member pointer
	auto& m = m_s_Instance->m_Textures;

	//try to find the keyValuePair
	auto keyValuePair = m.find(filename);


	if (keyValuePair != m.end())
	{
		//return the texture if found successfully 
		return keyValuePair->second;
	}
	else
	{
		//create a new texture and store it in the map if not found. 
		//Also returning it in the end.
		auto& texture = m[filename];
		texture.loadFromFile(filename);
		return texture;
	}
}
