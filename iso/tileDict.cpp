#include "tileDict.h"


iso::tileDict::tileDict(void)
{
}


iso::tileDict::~tileDict(void)
{
}


iso::tileType* iso::tileDict::allocateTile(std::string tileName)
{
	std::map<std::string, std::pair<int, iso::tileType*>>::iterator i = tiles.find(tileName);
	if(i == tiles.cend())
	{
		// tile doesn't exist
		return NULL;
	}
	if(i->second.first == 0)
	{
		//  hasn't been allocated yet
		i->second.second->loadTile();
	}
	i->second.first++;
	return i->second.second;
}

bool iso::tileDict::releaseTile(iso::tileType& whichTile)
{
	std::map<std::string, std::pair<int, iso::tileType*>>::iterator i = tiles.find(whichTile.getName());
	if(i == tiles.cend())
	{
		return false;
	}

	i->second.first--;

	if(i->second.first == 0)
	{
		// cleanup
		i->second.second->unloadTile();
	}
	return true;
}

bool iso::tileDict::addTile(const tileType& whichTile)
{
	std::map<std::string, std::pair<int, iso::tileType*>>::iterator i = tiles.find(whichTile.getName());
	if(i != tiles.cend())
	{
		// already exists
		return false;
	}
	tileType* tempTile = new tileType(whichTile);
	tiles.insert(std::pair<std::string, std::pair<int, iso::tileType*> >(tempTile->getName(), std::pair<int, iso::tileType*>(0, tempTile) ) );
	return true;
}