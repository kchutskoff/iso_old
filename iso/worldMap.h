#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdint.h>

namespace iso
{

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
		unsigned x_pos:11;
		unsigned y_pos:11;
		unsigned height:10;
		unsigned texture:10;
		unsigned draw_order:6;
		unsigned x_off:7;
		unsigned y_off:7;
		char textureName[12];
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
		unsigned tile_type:10;
		unsigned orientation:4;
		unsigned height:10;
		unsigned walkable:8;
		std::vector<worldObject> objects;
		char tileName[12];
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