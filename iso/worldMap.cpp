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
		x_size = EXTRACT(tempInt, Y_POS_BITS + TILE_COUNT_BITS, X_POS_BITS);
		y_size = EXTRACT(tempInt, TILE_COUNT_BITS, Y_POS_BITS);
		tile_count = EXTRACT(tempInt, 0, TILE_COUNT_BITS);

		// read next 32 bits of the header
		file.read((char*)&tempInt, sizeof(tempInt));
		object_count = EXTRACT(tempInt, TEXTURE_COUNT_BITS, OBJECT_COUNT_BITS);
		texture_count = EXTRACT(tempInt, 0, TEXTURE_COUNT_BITS);
		// calculate expected size of file (to make sure nothing is malformed)
		std::streamoff maxByteSize = ((X_POS_BITS + Y_POS_BITS + TILE_COUNT_BITS + OBJECT_COUNT_BITS + TEXTURE_COUNT_BITS) / 8 )
			+ (x_size * y_size * (TILE_COUNT_BITS + ORIENTATION_BITS + HEIGHT_BITS + WALKABLE_BITS) / 8 )
			+ (object_count * (X_POS_BITS + Y_POS_BITS + HEIGHT_BITS + TEXTURE_COUNT_BITS + DRAW_ORDER_BITS + X_OFF_BITS + Y_OFF_BITS) / 8 )
			+ (tile_count * TILE_NAME_LENGTH) + (texture_count * TEXTURE_NAME_LENGTH);
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

		std::streamoff tileLoc = file.tellg();
		// start with the dictionaries first
		std::streamoff dictLoc = ((X_POS_BITS + Y_POS_BITS + TILE_COUNT_BITS + OBJECT_COUNT_BITS + TEXTURE_COUNT_BITS) / 8 )
			+ (x_size * y_size * (TILE_COUNT_BITS + ORIENTATION_BITS + HEIGHT_BITS + WALKABLE_BITS) / 8 )
			+ (object_count * (X_POS_BITS + Y_POS_BITS + HEIGHT_BITS + TEXTURE_COUNT_BITS + DRAW_ORDER_BITS + X_OFF_BITS + Y_OFF_BITS) / 8 );
		file.seekg(dictLoc);

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

		file.seekg(tileLoc);
		
		// tiles
		// start from 0 as 0,0 always has to be a tile (otherwise there would be nothing, which isn't a map)
		for(unsigned int x = 0; x < x_size; ++x)
		{
			for(unsigned int y = 0; y < y_size; ++y)
			{
				// read in tile data
				file.read((char*)&tempInt, sizeof(tempInt));
				tiles[x][y].tile_type = EXTRACT(tempInt, WALKABLE_BITS + HEIGHT_BITS + ORIENTATION_BITS, TILE_COUNT_BITS);
				tiles[x][y].orientation = EXTRACT(tempInt, WALKABLE_BITS + HEIGHT_BITS, ORIENTATION_BITS);
				tiles[x][y].height = EXTRACT(tempInt, WALKABLE_BITS, HEIGHT_BITS);
				tiles[x][y].walkable = EXTRACT(tempInt, 0, WALKABLE_BITS);

				// check variable values
				if(tiles[x][y].tile_type >= tile_count)
				{
					//TODO: error bad tile type
					file.close();
					return false;
				}
				// get tile from dictionary
				strcpy_s(tiles[x][y].tileName, tileDict[tiles[x][y].tile_type].c_str());
				// update references
				changeTileIndex(tiles[x][y].tileName, 1);
			}
		}

		// objects
		worldObject tempObject;

		for(unsigned int i = 0; i < object_count; ++i)
		{
			file.read((char*)&tempInt, sizeof(tempInt));
			tempObject.x_pos = EXTRACT(tempInt, Y_POS_BITS + HEIGHT_BITS, X_POS_BITS);
			tempObject.y_pos = EXTRACT(tempInt, HEIGHT_BITS, Y_POS_BITS);
			tempObject.height = EXTRACT(tempInt, 0, HEIGHT_BITS);

			file.read((char*)&tempInt, sizeof(tempInt));
			tempObject.texture = EXTRACT(tempInt, Y_OFF_BITS + X_OFF_BITS + DRAW_ORDER_BITS, TEXTURE_COUNT_BITS);
			tempObject.draw_order = EXTRACT(tempInt, Y_OFF_BITS + X_OFF_BITS, DRAW_ORDER_BITS);
			tempObject.x_off = EXTRACT(tempInt, Y_OFF_BITS, X_OFF_BITS);
			tempObject.y_off = EXTRACT(tempInt, 0, Y_OFF_BITS);
			
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
			// load texture from dict
			strcpy_s(tempObject.textureName, textureDict[tempObject.texture].c_str());
			// update references
			changeTextureIndex(tempObject.textureName, 1);
			// add object to the tile
			tiles[tempObject.x_pos][tempObject.y_pos].objects.push_back(tempObject);
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
		uint32_t tempInt = 0;
		INSERT(tempInt, x_size, X_POS_BITS);
		INSERT(tempInt, y_size, Y_POS_BITS);
		INSERT(tempInt, tileDict.size(), TILE_COUNT_BITS);
		file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));
		
		tempInt = 0;
		INSERT(tempInt, object_count, OBJECT_COUNT_BITS);
		INSERT(tempInt, textureDict.size(), TEXTURE_COUNT_BITS);
		file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));
		// now write each tile
		for(unsigned int x = 0; x < x_size; ++x)
		{
			for(unsigned int y = 0; y < y_size; ++y)
			{
				tempInt = 0;
				INSERT(tempInt, tileDict[tiles[x][y].tileName], TILE_COUNT_BITS);
				INSERT(tempInt, tiles[x][y].orientation, ORIENTATION_BITS);
				INSERT(tempInt, tiles[x][y].height, HEIGHT_BITS);
				INSERT(tempInt, tiles[x][y].walkable, WALKABLE_BITS);
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
					tempInt = 0;
					INSERT(tempInt, tiles[x][y].objects[i].x_pos, X_POS_BITS);
					INSERT(tempInt, tiles[x][y].objects[i].y_pos, Y_POS_BITS);
					INSERT(tempInt, tiles[x][y].objects[i].height, HEIGHT_BITS);
					file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));

					tempInt = 0;
					INSERT(tempInt, tiles[x][y].objects[i].texture, TEXTURE_COUNT_BITS);
					INSERT(tempInt, tiles[x][y].objects[i].draw_order, DRAW_ORDER_BITS);
					INSERT(tempInt, tiles[x][y].objects[i].x_off, X_OFF_BITS);
					INSERT(tempInt, tiles[x][y].objects[i].y_off, Y_OFF_BITS);
					file.write(reinterpret_cast<const char *>(&tempInt), sizeof(tempInt));
				}
			}
		}
		// write tileDict
		for(I = tileDict.cbegin(); I != tileDict.cend(); ++I)
		{
			file.write(I->first.c_str(), TILE_NAME_LENGTH);
		}

		// write textureDict
		for(I = textureDict.cbegin(); I != textureDict.cend(); ++I)
		{
			file.write(I->first.c_str(), TEXTURE_NAME_LENGTH);
		}

		// make sure the file size is what we expected
		std::streamoff maxByteSize = ((X_POS_BITS + Y_POS_BITS + TILE_COUNT_BITS + OBJECT_COUNT_BITS + TEXTURE_COUNT_BITS) / 8 )
			+ (x_size * y_size * (TILE_COUNT_BITS + ORIENTATION_BITS + HEIGHT_BITS + WALKABLE_BITS) / 8 )
			+ (object_count * (X_POS_BITS + Y_POS_BITS + HEIGHT_BITS + TEXTURE_COUNT_BITS + DRAW_ORDER_BITS + X_OFF_BITS + Y_OFF_BITS) / 8 )
			+ (tile_count * TILE_NAME_LENGTH) + (texture_count * TEXTURE_NAME_LENGTH);
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
	// 0 to 2047, where 0,0 is 1 tile and 2047,2047 is 2048x2048 tiles
	// this is because a map can't have 0 tiles =D

	// is the range valid?
	if(x < X_POS_MAX && y < Y_POS_MAX)
	{
		// clear out objects if we are reducing size
		if(x < x_size)
		{
			if(y < y_size)
			{
				// shrink x and y
				// bottom part
				for(unsigned int tempX = 0; tempX <= x_size; ++tempX)
				{
					for(unsigned int tempY = y; tempY <= y_size; ++tempY)
					{
						changeTileIndex(tiles[tempX][tempY].tileName, -1);
						while(tiles[tempX][tempY].objects.size() > 0)
						{
							removeObject(tiles[tempX][tempY].objects.back());
						}
					}
				}
				// side part (minus corner)
				for(unsigned int tempX = x; tempX <= x_size; ++tempX)
				{
					for(unsigned int tempY = 0; tempY <= y; ++tempY)
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
				for(unsigned int tempX = x; tempX <= x_size; ++tempX)
				{
					for(unsigned int tempY = 0; tempY <= y_size; ++tempY)
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
			for(unsigned int tempX = 0; tempX <= x_size; ++tempX)
			{
				for(unsigned int tempY = y; tempY <= y_size; ++tempY)
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
		// we want 0 to x, so need to resize to x+1
		tiles.resize(x+1);
		for(unsigned int i = 0; i <= x; ++i){
			tiles[i].resize(y+1, worldTile(defaultTileName));
		}
		// calculate the total number of tile changes
		int tileAdditions = ((x+1)*(y+1))-((x_size+1)*(y_size+1));
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
	if(x > x_size || y > y_size)
	{
		// TODO: error not valid size
		return false;
	}
	if(whichObject.height >= HEIGHT_MAX)
	{
		// TODO: error not valid height
		return false;
	}
	if(whichObject.draw_order >= DRAW_ORDER_MAX)
	{
		// TODO: error not a valid draw order
		return false;
	}
	if(whichObject.x_off >= X_OFF_MAX || whichObject.y_off >= Y_OFF_MAX)
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