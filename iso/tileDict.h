#pragma once
#include <string>
#include <map>
#include "tileType.h"

namespace iso
{

class tileDict
{
private:
	std::map<std::string, std::pair<int, iso::tileType*>> tiles;
public:
	tileDict(void);
	~tileDict(void);

	tileType* allocateTile(std::string tileName);
	bool releaseTile(tileType& whichTile);

	bool addTile(const tileType& whichTile);
};

}