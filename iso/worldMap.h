#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdint.h>


namespace iso
{
#define EXTRACT(SRC, POS, BITS) ((SRC) >> (POS)) & ((1<<(BITS))-1) 
#define INSERT(DST, SRC, BITS) DST = ((DST)<<(BITS)) + (SRC)

	const unsigned int X_POS_BITS = 11;
	const unsigned int Y_POS_BITS = 11;
	const unsigned int X_POS_MAX = 2048;
	const unsigned int Y_POS_MAX = 2048;
	const unsigned int HEIGHT_BITS = 10;
	const unsigned int HEIGHT_MAX = 1024;
	const unsigned int TEXTURE_COUNT_BITS = 12;
	const unsigned int TEXTURE_COUNT_MAX = 4096;
	const unsigned int OBJECT_COUNT_BITS = 20;
	const unsigned int OBJECT_COUNT_MAX = 1048576;
	const unsigned int DRAW_ORDER_BITS = 6;
	const unsigned int DRAW_ORDER_MAX = 64;
	const unsigned int X_OFF_BITS = 7;
	const unsigned int Y_OFF_BITS = 7;
	const unsigned int X_OFF_MAX = 128;
	const unsigned int Y_OFF_MAX = 128;
	const unsigned int TILE_COUNT_BITS = 10;
	const unsigned int TILE_COUNT_MAX = 1024;
	const unsigned int ORIENTATION_BITS = 4;
	const unsigned int ORIENTATION_MAX = 16;
	const unsigned int WALKABLE_BITS = 8;
	const unsigned int WALKABLE_MAX = 256;
	
	const unsigned int TILE_NAME_LENGTH = 12;
	const unsigned int TEXTURE_NAME_LENGTH = 12;

class worldMap
{
private:
	unsigned int x_size;
	unsigned int y_size;
	unsigned int tile_count;
	unsigned int object_count;
	unsigned int texture_count;
	std::string defaultTileName;
	std::map<std::string, unsigned int> tileIndex;
	std::map<std::string, unsigned int> textureIndex;

public:

	class worldObject
	{
	public:
		worldObject():x_pos(0), y_pos(0), height(0), texture(0), draw_order(0), x_off(0), y_off(0)
		{textureName[0] = '\0';}

		worldObject(unsigned int x, unsigned int y, unsigned int height, unsigned int drawOrder, unsigned int xOffset, unsigned int yOffset, std::string textrName)
			:x_pos(x), y_pos(y), height(height), texture(0), draw_order(drawOrder), x_off(xOffset), y_off(yOffset)
		{strcpy_s(textureName, textrName.c_str());}

		bool operator==(const worldObject& other) const
		{return(x_pos == other.x_pos && y_pos == other.y_pos && draw_order == other.draw_order);}

		~worldObject(){};
		// using bitfields to save space
		unsigned x_pos:X_POS_BITS;
		unsigned y_pos:Y_POS_BITS;
		unsigned height:HEIGHT_BITS;
		unsigned texture:TEXTURE_COUNT_BITS;
		unsigned draw_order:DRAW_ORDER_BITS;
		unsigned x_off:X_OFF_BITS;
		unsigned y_off:Y_OFF_BITS;
		char textureName[TEXTURE_NAME_LENGTH];
	};

	class worldTile
	{
	public:
		worldTile():tile_type(0), orientation(0), height(0), walkable(0)
		{tileName[0] = '\0';}

		worldTile(std::string tileType, unsigned int orientation = 0, unsigned int height = 0, unsigned int walkable = 0xFF)
			:tile_type(0), orientation(orientation), height(height), walkable(walkable)
		{strcpy_s(tileName, tileType.c_str());}

		// using bitfields to save space
		unsigned tile_type:TILE_COUNT_BITS;
		unsigned orientation:ORIENTATION_BITS;
		unsigned height:HEIGHT_BITS;
		unsigned walkable:WALKABLE_BITS;
		std::vector<worldObject> objects;
		char tileName[TILE_NAME_LENGTH];
	};

	static const unsigned char walkable_TopLeft = 0x80;
	static const unsigned char walkable_TopMidLeft = 0x40;
	static const unsigned char walkable_TopMidRight = 0x20;
	static const unsigned char walkable_TopRight = 0x10;
	static const unsigned char walkable_BotRight = 0x08;
	static const unsigned char walkable_BotMidRight = 0x04;
	static const unsigned char walkable_BotMidLeft = 0x02;
	static const unsigned char walkable_BotLeft = 0x01;

private:

	std::vector<std::vector<worldTile>> tiles;	

	worldMap(void);
	void changeTileIndex(std::string index, int amount);
	void changeTextureIndex(std::string index, int amount);
	void clear(void);

public:
	worldMap(const std::string& defaultTile, unsigned int x, unsigned int y);
	worldMap(const std::string& fileName);
	~worldMap(void);

	bool loadFromFile(const std::string& fileName);
	bool writeToFile(const std::string& fileName) const;

	bool setSize(unsigned int x, unsigned int y);
	std::pair<unsigned int, unsigned int> getSize(void) const;

	bool addObject(unsigned int x, unsigned int y, unsigned int height, std::string texture, unsigned int drawOrder, unsigned int x_off, unsigned int y_off);
	bool removeObject(const worldObject& whichObject);
	bool updateObject(const worldObject& whichObject);

	std::vector<worldObject> getObjects(unsigned int x, unsigned int y) const;

	bool setTile(unsigned int x, unsigned int y, std::string tileType, unsigned int orientation, unsigned int height, unsigned int walkable);
	worldTile getTile(unsigned int x, unsigned int y) const;
};

}