#pragma once

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

	bool loadTextures(std::string filelist);

	texture* allocTexture(std::string name);
	bool releaseTexture(texture* t);


};

}