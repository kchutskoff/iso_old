#pragma once
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <algorithm>
#include <exception>
#include <list>


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

	template<class T1, class T2>
	class XY: std::pair<T1, T2> {
	public:
		XY(T1 x, T2 y): std::pair<T1, T2>(x, y), x(first), y(second) { };
		T1 &x;
		T2 &y;
		void swap(void){std::pair<T1, T2>::swap();};
	};

	typedef XY<unsigned int, unsigned int> size;
	typedef XY<unsigned int, unsigned int> location;

	class worldObject
	{
	private:
		static std::queue<unsigned int> unusedIDs;
		static unsigned int IDcount;

		static unsigned int newID(void);
		static void releaseID(unsigned int ID);

		// using bitfields to save space
		unsigned x_pos:X_POS_BITS;
		unsigned y_pos:Y_POS_BITS;
		unsigned height:HEIGHT_BITS;
		unsigned texture:TEXTURE_COUNT_BITS;
		unsigned draw_order:DRAW_ORDER_BITS;
		unsigned x_off:X_OFF_BITS;
		unsigned y_off:Y_OFF_BITS;
		char textureName[TEXTURE_NAME_LENGTH];
		unsigned int ID;
		worldMap* parentMap;
		
		// Shouldn't be able to create worldObjects outside of the world map class.
		worldObject():x_pos(0), y_pos(0), height(0), texture(0), draw_order(0), x_off(0), y_off(0)
		{textureName[0] = '\0'; ID = worldObject.newID();}

		worldObject(worldMap& parent, location loc, unsigned int height, unsigned int xOffset, unsigned int yOffset, std::string textrName)
			:parentMap(& parent), x_pos(loc.x), y_pos(loc.y), height(height), texture(0), draw_order(0), x_off(xOffset), y_off(yOffset)
		{strcpy_s(textureName, textrName.c_str()); ID = worldObject.newID(); }

	public:
		
		enum drawOrder{
			moveToFront,
			moveForward,
			moveBackward,
			moveToBack,
		};

		bool operator==(const worldObject& other) const
		{return ID == other.ID;}

		bool operator<(const worldObject& other) const
		{return(x_pos < other.x_pos || y_pos < other.y_pos || draw_order < other.draw_order);}

		~worldObject(){};

		// throws range_error on failure
		void setXOffset(unsigned int xOff);
		unsigned int getXOffset(void) const;

		// throws range_error on failure
		void setYOffset(unsigned int xOff);
		unsigned int getYOffset(void) const;

		// throws range_error on failure
		void setHeight(unsigned int height);
		unsigned int getHeight(void) const;

		// throws runtime_error on failure
		void setTextureName(const std::string& textureName);
		std::string getTextureName(void) const;

		void changeDrawOrder(const enum drawOrder& whichOrder);
		unsigned int getDrawOrder(void) const;

		void changePosition(const location& newLoc);
		location getPosition(void);

		unsigned int getID(void) const;

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
		// although this vector can hold up to 64 items (based on draw_order), it is not 64 elements in size
		// this is because most tiles won't even have close to 64 objects on them, so why allocate the space?

		std::list<worldObject> objects;

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

	// no default constructor allowed as we need at least a default tile
	worldMap(void);

	// helper functions to do with maintainning the number of tiles and textures being used
	void changeTileIndex(std::string index, int amount);
	void changeTextureIndex(std::string index, int amount);

	// helper function that clears the map
	void clear(void);

public:
	worldMap(const std::string& defaultTile, const size& newSize);
	worldMap(const std::string& fileName);
	~worldMap(void);

	bool loadFromFile(const std::string& fileName);
	bool writeToFile(const std::string& fileName) const;

	bool setSize(const size& newSize);
	size getSize(void) const;

	bool setDefaultTile(const std::string& defaultTile);
	std::string getDefaultTile(void) const;

	worldObject addObject(unsigned int x, unsigned int y, unsigned int drawOrder, unsigned int height, std::string texture, unsigned int x_off, unsigned int y_off);
	bool removeObject(worldObject& whichObject);
	bool updateObject(const worldObject& whichObject);

	std::vector<worldObject> getObjects(unsigned int x, unsigned int y) const;
	worldObject getObject(unsigned int x, unsigned int y, unsigned int drawOrder);

	bool setTile(unsigned int x, unsigned int y, std::string tileType, unsigned int orientation, unsigned int height, unsigned int walkable);
	worldTile getTile(unsigned int x, unsigned int y) const;

	
};

}