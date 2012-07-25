#include "worldMap.h"

unsigned int iso::worldMap::worldObject::newID(void)
{
	if(unusedIDs.size() > 0)
	{
		unsigned int temp = unusedIDs.front();
		unusedIDs.pop();
		return temp;
	}else
	{
		return ++IDcount;
	}
}

void iso::worldMap::worldObject::releaseID(unsigned int ID)
{
	unusedIDs.push(ID);
}

// throws range_error on failure
void iso::worldMap::worldObject::setXOffset(unsigned int xOff)
{
	if(xOff >= X_OFF_MAX)
	{
		std::stringstream temp;
		temp << "Range Error: X Offset (" << xOff <<") larger than X_OFF_MAX (" << X_OFF_MAX <<") in function iso::worldMap::worldObject::setXOffset!";
		throw std::range_error(temp.str());
	}
	x_off = xOff;
}

unsigned int iso::worldMap::worldObject::getXOffset(void) const
{
	return x_off;
}

// throws range_error on failure
void iso::worldMap::worldObject::setYOffset(unsigned int yOff)
{
	if(yOff >= Y_OFF_MAX)
	{
		std::stringstream temp;
		temp << "Range Error: Y Offset (" << yOff <<") larger than Y_OFF_MAX (" << Y_OFF_MAX <<") in function iso::worldMap::worldObject::setYOffset!";
		throw std::range_error(temp.str());
	}
	y_off = yOff;
}

unsigned int iso::worldMap::worldObject::getYOffset(void) const
{
	return y_off;
}

// throws range_error on failure
void iso::worldMap::worldObject::setHeight(unsigned int newHeight)
{
	if(newHeight >= HEIGHT_MAX)
	{
		std::stringstream temp;
		temp << "Range Error: newHeight (" << newHeight <<") larger than HEIGHT_MAX (" << HEIGHT_MAX <<") in function iso::worldMap::worldObject::setHeight!";
		throw std::range_error(temp.str());
	}
	height = newHeight;
}
	
unsigned int iso::worldMap::worldObject::getHeight(void) const
{
	return height;
}

// throws runtime_error on failure
void iso::worldMap::worldObject::setTextureName(const std::string& newTextureName){
	if(newTextureName.size() >= TEXTURE_NAME_LENGTH)
	{
		std::stringstream temp;
		temp << "Runtime Error: newTextureName (" << newTextureName <<") is longer than TEXTURE_NAME_LENGTH (" << TEXTURE_NAME_LENGTH <<") in function iso::worldMap::worldObject::setTextureName!";
		throw std::runtime_error(temp.str());
	}
	strcpy_s(textureName, newTextureName.c_str());
}

std::string  iso::worldMap::worldObject::getTextureName(void) const
{
	return textureName;
}

void  iso::worldMap::worldObject::changeDrawOrder(const enum drawOrder& whichOrder)
{
	// get a pointer to the object vector (so we don't keep having to use parentMap->tiles...)
	std::list<worldObject>* objs = & parentMap->tiles[x_pos][y_pos].objects;
	std::list<worldObject>::iterator I = std::lower_bound(objs->begin(), objs->end(), this);
	
	if(I != objs->end() && *I == *this){
		// object isn't in the map... something went terribly wrong
		std::stringstream temp;
		temp << "FATAL ERROR: Object does not exist at tile ("<<x_pos<<","<<y_pos<<") in function iso::worldMap::worldObject::changeDrawOrder!";
		throw std::runtime_error(temp.str());
	}
	std::list<worldObject>::iterator I2;
	switch(whichOrder){
	case moveBackward:
		I2 = ++I;
		// if we aren't already at the end
		if(I2 != objs->end())
		{
			std::iter_swap(I2, I);
			I->draw_order++;
			I2->draw_order--;
		}
		break;
	case moveForward:
		// make sure we aren't at the front already
		if(I != objs->begin())
		{
			I2 = --I;
			std::iter_swap(I2, I);
			I->draw_order--;
			I2->draw_order++;
		}
		break;
	case moveToBack:
		// if we aren't at the end already
		if(I != ++objs->end())
		{
			// then we need to bring I to the back, but first update draw orders
			// we decrease the draw order for all objects after our object (as they are moving forward)
			std::for_each(I, objs->end(), [&](worldObject& obj){obj.draw_order--;});
			// and update our draw order
			I->draw_order = objs->size() - 1;
			// now place a new copy of the object at the end;
			// explicitly call the copy constructor
			objs->push_back(worldObject(*I));
			// and remove the old object
			objs->erase(I); // dereferences I
		}
		break;
	case moveToFront:
		// if we aren't at the front already
		if(I != objs->begin())
		{
			// then we need to bring I to the front, but first update draw orders
			// we increase the draw order for all objects between the front and our object (as they are moving backward)
			std::for_each(objs->begin(), I, [&](worldObject& obj){obj.draw_order++;});
			// and update our draw order
			I->draw_order = 0;
			// now place a new copy of the object at the end;
			// explicitly call the copy constructor
			objs->push_front(worldObject(*I));
			// and remove the old object
			objs->erase(I); // dereferences I
		}
		break;
	default:
		std::stringstream temp;
		temp << "Error: Unknown enum drawOrder (" << whichOrder <<") passed in function iso::worldMap::worldObject::changeDrawOrder!";
		throw std::runtime_error(temp.str());
	}
}

unsigned int iso::worldMap::worldObject::getDrawOrder(void) const
{
	return draw_order;
}

void iso::worldMap::worldObject::changePosition(const location& newLoc)
{
	if(newLoc.x >= parentMap->x_size || newLoc.y >= parentMap->y_size)
	{
		std::stringstream temp;
		temp <<"Error: newLoc ("<<newLoc.x<<","<<newLoc.y<<") is outside the the map's size!"<<parentMap->x_size<<","<<parentMap->y_size
			<<") in function iso::worldMap::worldObject::changePosition!";
		throw std::range_error(temp.str());
	}
	if(parentMap->tiles[newLoc.x][newLoc.y].objects.size() >= DRAW_ORDER_MAX)
	{
		std::stringstream temp;
		temp << "Error: tile at newLoc ("<<newLoc.x<<","<<newLoc.y<<") already has the maximum number of objects in function iso::worldMap::worldObject::changePosition!";
		throw std::runtime_error(temp.str());
	}
	// all checks out, update our objects position
	//
	// TODO: FINISH THIS
	//
	//
}

iso::worldMap::location iso::worldMap::worldObject::getPosition(void){
	return location(x_pos, y_pos);
}

unsigned int iso::worldMap::worldObject::getID(void) const{
	return ID;
}

iso::worldMap::worldMap(void)
{
}


iso::worldMap::~worldMap(void)
{
}

iso::worldMap::worldMap(const std::string& defaultTile, const iso::worldMap::size& newSize)
{
	defaultTileName = defaultTile;
	x_size = 0;
	y_size = 0;
	tile_count = 0;
	object_count = 0;
	texture_count = 0;
	try
	{
		setSize(newSize);
	}catch(std::range_error& e)
	{
		// pass along the error, tacking on some additional information
		std::stringstream temp;
		temp << e.what() << " in function iso::worldMap::worldMap!";
		throw std::range_error(temp.str());
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

bool iso::worldMap::updateObjectHeight(unsigned int x, unsigned int y, unsigned int drawOrder, unsigned int newHeight)
{
	if(x >= X_POS_MAX || y >= Y_POS_MAX)
	{
		return false;
	}
	if(newHeight >= HEIGHT_MAX)
	{
		return false;
	}
	for(std::vector<worldObject>::iterator I = tiles[x][y].objects.begin(); I != tiles[x][y].objects.end(); ++I)
	{
		if(I->x_pos == x && I->y_pos == y && I->draw_order == drawOrder)
		{
			I->height = newHeight;
			return true;
		}
	}
	// can't find it...
	// TODO: error missing object
	return false;
}

bool iso::worldMap::updateObjectTexture(unsigned int x, unsigned int y, unsigned int drawOrder, std::string texture)
{
	if(x >= X_POS_MAX || y >= Y_POS_MAX)
	{
		return false;
	}
	for(std::vector<worldObject>::iterator I = tiles[x][y].objects.begin(); I != tiles[x][y].objects.end(); ++I)
	{
		if(I->x_pos == x && I->y_pos == y && I->draw_order == drawOrder)
		{
			if(strcmp(I->textureName, texture.c_str()) != 0)
			{
				// new texture
				changeTextureIndex(I->textureName, -1);
				changeTextureIndex(texture, 1);
				// and copy the name
				strcpy_s(I->textureName, texture.c_str());
			}
			return true;
		}
	}
	return false;
	// not found
}

bool iso::worldMap::updateObjectOffset(unsigned int x, unsigned int y, unsigned int drawOrder, unsigned int x_off, unsigned int y_off)
{
	if(x >= X_POS_MAX || y >= Y_POS_MAX)
	{
		return false;
	}
	if(x_off >= X_OFF_MAX || y_off >= Y_OFF_MAX)
	{
		return false;
	}
	for(std::vector<worldObject>::iterator I = tiles[x][y].objects.begin(); I != tiles[x][y].objects.end(); ++I)
	{
		if(I->x_pos == x && I->y_pos == y && I->draw_order == drawOrder)
		{
			I->x_off = x_off;
			I->y_off = y_off;
			return true;
		}
	}
}

bool iso::worldMap::updateObjectDrawOrder(unsigned int x, unsigned int y, unsigned int oldDrawOrder, unsigned int newDrawOrder)
{
	if(x >= X_POS_MAX || y >= Y_POS_MAX)
	{
		return false;
	}
	std::vector<worldObject>::iterator I = std::lower_bound(tiles[x][y].objects.begin(), tiles[x][y].objects.end(), worldObject(x, y, oldDrawOrder, 0, 0, 0, ""));
	if(I != tiles[x][y].objects.end())
	{
		std::vector<worldObject>::iterator I2 = std::lower_bound(tiles[x][y].objects.begin(), tiles[x][y].objects.end(), worldObject(x, y, newDrawOrder, 0, 0, 0, ""));
		if( I->draw_order != newDrawOrder){
			// alright to place
			tiles[x][y].objects.insert(I2, 


	}
		/*
	for(std::vector<worldObject>::iterator I = tiles[x][y].objects.begin(); I != tiles[x][y].objects.end(); ++I)
	{
		if(I->x_pos == x && I->y_pos == y && I->draw_order == oldDrawOrder)
		{
			// now we want to see if the draw order is available
			std::vector<worldObject>::iterator otherI;
			if(oldDrawOrder < newDrawOrder){
				otherI = std::find(tiles[x][y].objects.begin(), I, worldObject(x, y, 0, newDrawOrder, 0, 0, ""));
			}else{
				otherI = std::binary_search(st
		}

	}*/
}

bool iso::worldMap::addObject(unsigned int x, unsigned int y, unsigned int drawOrder, unsigned int height, std::string texture, unsigned int x_off, unsigned int y_off)
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

bool iso::worldMap::addObject(const worldObject& newObject)
{
	return true;
}

bool iso::worldMap::removeObject(const worldObject& whichObject)
{
	unsigned int x = whichObject.x_pos;
	unsigned int y = whichObject.y_pos;
	if(x < x_size && y < y_size)
	{
		// look for the object
		for(std::vector<worldObject>::iterator I = tiles[x][y].objects.begin(); I != tiles[x][y].objects.end(); ++I)
		{
			if(*I == whichObject)
			{
				// dereference the texture
				changeTextureIndex(I->textureName, -1);
				// remove the object
				tiles[x][y].objects.erase(I);
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

