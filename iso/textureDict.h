#pragma once

// TODO: Make a better texture list format that supports loading animated textures.

#include "texture.h"
#include <map>
#include <cstdio>
#include <string>
#include <iterator>

namespace iso
{

class textureDict
{
private:
	std::map<std::string, std::pair<int, iso::texture*> > dict;
public:
	textureDict(void);
	~textureDict(void);

	bool loadTextureList(std::string filelist);

	bool addTexture(const texture& whichTexture);

	texture* allocTexture(std::string name);
	bool releaseTexture(texture& t);
};

}