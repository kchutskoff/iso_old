#include "worldMap.h"


iso::worldMap::worldMap(void)
{
}


iso::worldMap::~worldMap(void)
{
}

iso::worldMap::worldMap(const std::string& defaultTile, unsigned int x, unsigned int y)
{
	defaultTileName = defaultTile;
	x_size = 0;
	y_size = 0;
	tile_count = 0;
	object_count = 0;
	texture_count = 0;
	if(!setSize(x, y))
	{
		// problem with size
		setSize(1,1);
	}
}


iso::worldMap::worldMap(const std::string& fileName)
{
	loadFromFile(fileName);
}

bool iso::worldMap::loadFromFile(const std::string& fileName)
{
	clear();
	std::fstream file;
	file.open(fileName.c_str(), std::fstream::binary | std::fstream::in | std::fstream::ate);
	if(file.is_open())
	{
		std::streamoff fileByteSize = file.tellg();
		// holds the IDs until we have strings
		std::vector<std::string> tileDict;
		std::vector<std::string> textureDict;
		// read header
		file.seekg(std::fstream::beg);
		// force 32 bits, as I don't know what the future holds
		uint32_t tempInt = 0;
		file.read((char*)&tempInt, sizeof(tempInt));
		x_size = (tempInt >> 21);
		tempInt = tempInt & ~0xFFE00000; // make sure the first 11 bits are 0
		y_size = (tempInt >> 10);
		tempInt = tempInt & ~0xFFFFFC00; // make sure the first 22 bits are 0
		tile_count = tempInt;
		// read next 32 bits of the header
		file.read((char*)&tempInt, sizeof(tempInt));
		object_count = (tempInt >> 12);
		tempInt = tempInt & ~0xFFFFF000; // make sure the first 20 bits are 0;
		texture_count = tempInt;
		// calculate expected size of file (to make sure nothing is malformed)
		std::streamoff maxByteSize = 8 + (x_size * y_size * 4) + (object_count * 8) + (tile_count * 12) + (texture_count * 12);
		if(fileByteSize != maxByteSize)
		{
			// bad file
			//TODO: error file possibly corrupt, doesn't match specification
			file.close();
			return false;
		}
		// set vector sizes so we don't have data segmentation and costly resizes
		tiles.resize(x_size, std::vector<worldTile>(y_size));
		tileDict.resize(tile_count, "");
		textureDict.resize(texture_count, "");
		// time to read the data
		
		// tiles
		// start from 0 as 0,0 always has to be a tile (otherwise there would be nothing, which isn't a map)
		for(unsigned int x = 0; x < x_size; ++x)
		{
			for(unsigned int y = 0; y < y_size; ++y)
			{
				// read in tile data
				file.read((char*)&tempInt, sizeof(tempInt));
				tiles[x][y].tile_type = (tempInt >> 22);
				tempInt = tempInt & ~0xFF800000; // make sure the first 9 bits are 0;
				tiles[x][y].orientation = (tempInt >> 18);
				tempInt = tempInt & ~0xFFFC0000; // make sure the first 14 bits are 0;
				tiles[x][y].height = (tempInt >> 8);
				tempInt = tempInt & ~0xFFFFFF00; // make sure the first 24 bits are 0;
				tiles[x][y].walkable = tempInt;

				// check variable values
				if(tiles[x][y].tile_type >= tile_count)
				{
					//TODO: error bad tile type
					file.close();
					return false;
				}
			}
		}

		// objects
		worldObject tempObject;

		for(unsigned int i = 0; i < object_count; ++i)
		{
			file.read((char*)&tempInt, sizeof(tempInt));
			tempObject.x_pos = (tempInt >> 21);
			tempInt = tempInt & ~0xFFE00000;
			tempObject.y_pos = (tempInt >> 10);
			tempInt = tempInt & ~0xFFFFFC00;
			tempObject.height = tempInt;

			file.read((char*)&tempInt, sizeof(tempInt));
			tempObject.texture = (tempInt >> 20);
			tempInt = tempInt & ~0xFFF00000;
			tempObject.draw_order = (tempInt >> 14);
			tempInt = tempInt & ~0xFFFFC000;
			tempObject.x_off = (tempInt >> 7);
			tempInt = tempInt & ~0xFFFFFF80;
			tempObject.y_off = tempInt;
			
			// check variable values
			if(tempObject.x_pos >= x_size || tempObject.y_pos >= y_size)
			{
				// TODO: error bad size
				file.close();
				return false;
			}
			if(tempObject.texture >= texture_count)
			{
				// TODO: error bad texture
				file.close();
				return false;
			}

			// add object to the tile
			tiles[tempObject.x_pos][tempObject.y_pos].objects.push_back(tempObject);
		}
		// tileDict
		// start from 0
		uint8_t buffer[13];
		for(unsigned int i = 0; i < tile_count; ++i)
		{
			file.read((char*)buffer, 12);
			tileDict[i] = (char*)buffer;
		}

		// textureDict
		// start from 0
		for(unsigned int i = 0; i < texture_count; ++i)
		{
			file.read((char*)buffer, 12);
			textureDict[i] = (char*)buffer;
		}
		// default tile is at 0
		defaultTileName = tileDict[0];

		// now to use the dictionary to set the textures and tiles
		for(unsigned int x = 0; x < x_size; ++x)
		{
			for(unsigned int y = 0; y < y_size; ++y)
			{
				// tiles
				// count the number of references
				tileIndex[tileDict[tiles[x][y].tile_type]] += 1;
				// set the string name
				strcpy_s(tiles[x][y].tileName, tileDict[tiles[x][y].tile_type].c_str());
				// objects
				for(unsigned int i = 0; i < tiles[x][y].objects.size(); ++i)
				{
					// count the number of references
					textureIndex[textureDict[tiles[x][y].objects[i].texture]] += 1;
					strcpy_s(tiles[x][y].objects[i].textureName, textureDict[tiles[x][y].objects[i].texture].c_str());
				}
			}
		}
		if(tileIndex.size() != tile_count)
		{
			// tile size isn't correct?
			tile_count = tileIndex.size();
		}
		if(textureIndex.size() != texture_count)
		{
			// texture size isn't correct?
			texture_count = textureIndex.size();
		}

		return true;
	}else
	{
		//TODO: error file couldn't open
		return false;
	}
}

bool iso::worldMap::writeToFile(const std::string& fileName) const
{
	std::fstream file;
	file.open(fileName, std::fstream::binary | std::fstream::out | std::fstream::trunc);
	if(file.is_open())
	{
		std::map<std::string, unsigned int> tileDict;
		std::map<std::string, unsigned int> textureDict;
		// construct dictionaries for tiles and textures
		std::map<std::string, unsigned int>::const_iterator I;
		tileDict[defaultTileName] = 0;
		unsigned int tempIndex = 1;
		for(I = tileIndex.cbegin(); I != tileIndex.cend(); ++I)
		{
			if(I->first != defaultTileName)
			{
				tileDict[I->first] = tempIndex++;
			}
		}
		tempIndex = 0;
		for(I = textureIndex.cbegin(); I != textureIndex.cend(); ++I)
		{
			textureDict[I->first] = tempIndex++;
		}
		// write the header
		uint32_t tempInt;
		tempInt = x_size;
		tempInt = (tempInt << 11);
		tempInt += y_size;
		tempInt = (tempInt << 10);
		tempInt += tileDict.size();
		file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));

		tempInt = object_count;
		tempInt = (tempInt << 12);
		tempInt += textureDict.size();
		file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));
		// now write each tile
		for(unsigned int x = 0; x < x_size; ++x)
		{
			for(unsigned int y = 0; y < y_size; ++y)
			{
				tempInt = tileDict[tiles[x][y].tileName];
				tempInt = (tempInt << 4);
				tempInt += tiles[x][y].orientation;
				tempInt = (tempInt << 10);
				tempInt += tiles[x][y].height;
				tempInt = (tempInt << 8);
				tempInt += tiles[x][y].walkable;
				file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));
			}
		}
		// write objects
		for(unsigned int x = 0; x < x_size; ++x)
		{
			for(unsigned int y = 0; y < y_size; ++y)
			{
				for(unsigned int i = 0; i < tiles[x][y].objects.size(); ++i)
				{
					tempInt = tiles[x][y].objects[i].x_pos;
					tempInt = (tempInt << 11);
					tempInt += tiles[x][y].objects[i].y_pos;
					tempInt = (tempInt << 10);
					tempInt += tiles[x][y].objects[i].height;
					file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));

					tempInt = tiles[x][y].objects[i].texture;
					tempInt = (tempInt << 6);
					tempInt += tiles[x][y].objects[i].draw_order;
					tempInt = (tempInt << 7);
					tempInt += tiles[x][y].objects[i].x_off;
					tempInt = (tempInt << 7);
					tempInt += tiles[x][y].objects[i].y_off;
					file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));
				}
			}
		}
		// write tileDict
		for(I = tileDict.cbegin(); I != tileDict.cend(); ++I)
		{
			file.write(I->first.c_str(), 12);
		}

		// write textureDict
		for(I = textureDict.cbegin(); I != textureDict.cend(); ++I)
		{
			file.write(I->first.c_str(), 12);
		}

		// make sure the file size is what we expected
		std::streamoff maxByteSize = 8 + (x_size * y_size * 4) + (object_count * 8) + (tileDict.size() * 12) + (textureDict.size() * 12);
		std::streamoff fileByteSize = file.tellg();
		file.close();
		if(maxByteSize != fileByteSize)
		{
			// TODO: writing error (shouldn't happen)
			return false;
		}else
		{
			return true;
		}

	}else{
		//TODO: error could not create/open file for writing
		return false;
	}
}

bool iso::worldMap::setSize(unsigned int x, unsigned int y)
{
	// is the range valid?
	if(x < 2048 && y < 2048 && x > 0 && y > 0)
	{
		// clear out objects if we are reducing size
		if(x < x_size)
		{
			if(y < y_size)
			{
				// shrink x and y
				// bottom part
				for(unsigned int tempX = 0; tempX < x_size; ++tempX)
				{
					for(unsigned int tempY = y; tempY < y_size; ++tempY)
					{
						changeTileIndex(tiles[tempX][tempY].tileName, -1);
						while(tiles[tempX][tempY].objects.size() > 0)
						{
							removeObject(tiles[tempX][tempY].objects.back());
						}
					}
				}
				// side part (minus corner)
				for(unsigned int tempX = x; tempX < x_size; ++tempX)
				{
					for(unsigned int tempY = 0; tempY < y; ++tempY)
					{
						changeTileIndex(tiles[tempX][tempY].tileName, -1);
						while(tiles[tempX][tempY].objects.size() > 0)
						{
							removeObject(tiles[tempX][tempY].objects.back());
						}
					}
				}

			}else
			{
				// shrink x only
				for(unsigned int tempX = x; tempX < x_size; ++tempX)
				{
					for(unsigned int tempY = 0; tempY < y_size; ++tempY)
					{
						changeTileIndex(tiles[tempX][tempY].tileName, -1);
						while(tiles[tempX][tempY].objects.size() > 0)
						{
							removeObject(tiles[tempX][tempY].objects.back());
						}
					}
				}
			}
		}else if(y < y_size)
		{
			// shrink y only
			for(unsigned int tempX = 0; tempX < x_size; ++tempX)
			{
				for(unsigned int tempY = y; tempY < y_size; ++tempY)
				{
					changeTileIndex(tiles[tempX][tempY].tileName, -1);
					while(tiles[tempX][tempY].objects.size() > 0)
					{
						removeObject(tiles[tempX][tempY].objects.back());
					}
				}
			}
		}

		// done clearing (if needed), resize the 2d vector		

		tiles.resize(x);
		for(unsigned int i = 0; i < x; ++i){
			tiles[i].resize(y, worldTile(defaultTileName));
		}
		// calculate the total number of tile changes
		int tileAdditions = (x*y)-(x_size*y_size);
		if(tileAdditions >= 0)
		{
			// we added tiles with the default tile type
			changeTileIndex(defaultTileName, tileAdditions);
		}
		x_size = x;
		y_size = y;
		return true;
	}else
	{
		//TODO: error not valid range
		return false;
	}
}
std::pair<unsigned int, unsigned int> iso::worldMap::getSize(void) const
{
	return std::pair<unsigned int, unsigned int>(x_size, y_size);
}

bool iso::worldMap::updateObject(const worldObject& whichObject)
{
	unsigned int x = whichObject.x_pos;
	unsigned int y = whichObject.y_pos;
	if(x >= x_size || y >= y_size)
	{
		// TODO: error not valid size
		return false;
	}
	if(whichObject.height >= 1024)
	{
		// TODO: error not valid height
		return false;
	}
	if(whichObject.draw_order >= 64)
	{
		// TODO: error not a valid draw order
		return false;
	}
	if(whichObject.x_off >= 128 || whichObject.y_off >= 128)
	{
		// TODO: error not a valid offset
		return false;
	}
	// otherwise all good
	// look for the object
	for(unsigned int i = 0; i < tiles[x][y].objects.size(); ++i)
	{
		if(tiles[x][y].objects[i] == whichObject)
		{
			tiles[x][y].objects[i].draw_order = whichObject.draw_order;
			tiles[x][y].objects[i].height = whichObject.height;
			tiles[x][y].objects[i].x_off = whichObject.x_off;
			tiles[x][y].objects[i].y_off = whichObject.y_off;
			if(strcmp(tiles[x][y].objects[i].textureName, whichObject.textureName) != 0)
			{
				// updating texture as well
				// dereference the texture
				changeTextureIndex(tiles[x][y].objects[i].textureName, -1);
				changeTextureIndex(whichObject.textureName, 1);
				// and copy the name
				strcpy_s(tiles[x][y].objects[i].textureName, whichObject.textureName);
			}
			return true;
		}
	}
	// can't find it...
	// TODO: error missing object
	return false;
}

bool iso::worldMap::addObject(unsigned int x, unsigned int y, unsigned int height, std::string texture, unsigned int drawOrder, unsigned int x_off, unsigned int y_off)
{
	if(x >= x_size || y >= y_size)
	{
		// TODO: error not valid size
		return false;
	}
	if(height >= 1024)
	{
		// TODO: error not valid height
		return false;
	}
	if(texture.size() >= 12)
	{
		// TODO: error not a valid texture
		return false;
	}
	if(drawOrder >= 64)
	{
		// TODO: error not a valid draw order
		return false;
	}
	if(x_off >= 128 || y_off >= 128)
	{
		// TODO: error not a valid offset
		return false;
	}
	if(tiles[x][y].objects.size() >= 64)
	{
		// TODO: error no more objects on tile
		return false;
	}
	// otherwise all good
	tiles[x][y].objects.push_back(worldObject(x, y, height, drawOrder, x_off, y_off, texture));	
	changeTextureIndex(texture, 1);
	object_count++;
	return true;
}
bool iso::worldMap::removeObject(const worldObject& whichObject)
{
	unsigned int x = whichObject.x_pos;
	unsigned int y = whichObject.y_pos;
	if(x < x_size && y < y_size)
	{
		// look for the object
		for(unsigned int i = 0; i < tiles[x][y].objects.size(); ++i)
		{
			if(tiles[x][y].objects[i] == whichObject)
			{
				// dereference the texture
				changeTextureIndex(whichObject.textureName, -1);
				// swap with last element
				tiles[x][y].objects[i] = tiles[x][y].objects[tiles[x][y].objects.size()-1];
				// remove last element
				tiles[x][y].objects.pop_back();
				object_count--;
				
				return true;
			}
		}
		// TODO: error doesn't exist
		return false;
	}else
	{
		// TODO: error bad object
		return false;
	}	
}

std::vector<iso::worldMap::worldObject> iso::worldMap::getObjects(unsigned int x, unsigned int y) const
{
	return tiles[x][y].objects;
}

bool iso::worldMap::setTile(unsigned int x, unsigned int y, std::string tileType, unsigned int orientation, unsigned int height, unsigned int walkable)
{
	if(x >= x_size || y >= y_size)
	{
		// TODO: error not valid size
		return false;
	}
	if(tileType.size() >= 12)
	{
		// TODO: error not valid tile type
		return false;
	}
	if(orientation >= 16)
	{
		// TODO: error not a valid orientation
		return false;
	}
	if(height >= 1024)
	{
		// TODO: error not a valid height
		return false;
	}
	if(walkable >= 256)
	{
		// TODO: error not a walkable value
		return false;
	}
	// otherwise all good
	tiles[x][y].height = height;
	// check if new tile type
	if(strcmp(tiles[x][y].tileName, tileType.c_str()) != 0)
	{
		// dereference old tile
		changeTileIndex(tiles[x][y].tileName, -1);
		changeTileIndex(tileType, 1);
		// and copy the new one
		strcpy_s(tiles[x][y].tileName, tileType.c_str());
	}
	
	tiles[x][y].walkable = walkable;
	tiles[x][y].orientation = orientation;
	return true;
}
iso::worldMap::worldTile iso::worldMap::getTile(unsigned int x, unsigned int y) const
{
	return tiles[x][y];
}


void iso::worldMap::changeTileIndex(std::string index, int amount)
{
	std::map<std::string, unsigned int>::iterator I = tileIndex.find(index);
	if(I != tileIndex.end())
	{
		I->second += amount; // amount could be negative
		if(I->second <= 0){
			// if its 0, erase it
			tileIndex.erase(I);
			tile_count--;
		}
	}else
	{
		// not found
		if(amount > 0){
			// this will create the new index
			tileIndex[index] = amount;
			tile_count++;
		}else
		{
			// TODO: error tried to decrease unindexed tile
		}
	}
}

void iso::worldMap::changeTextureIndex(std::string index, int amount)
{
	std::map<std::string, unsigned int>::iterator I = textureIndex.find(index);
	if(I != textureIndex.end())
	{
		I->second += amount; // amount could be negative
		if(I->second <= 0){
			// if its 0, erase it
			textureIndex.erase(I);
			texture_count--;
		}
	}else
	{
		// not found
		if(amount > 0){
			// this will create the new index
			textureIndex[index] = amount;
			texture_count++;
		}else
		{
			// TODO: error tried to decrease unindexed texture
		}
	}
}

void iso::worldMap::clear()
{
	// clear out everything
	tiles.clear();
	tiles.shrink_to_fit();
	tileIndex.clear();
	textureIndex.clear();
	tile_count = 0;
	object_count = 0;
	texture_count = 0;
	x_size = 0;
	y_size = 0;
	defaultTileName = "";
}