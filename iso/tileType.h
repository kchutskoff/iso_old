#pragma once
#include <string>
#include <SFML\Graphics.hpp>

namespace iso
{
	const unsigned int TILE_SHEET_SIZE = 512;
	const unsigned int TILE_SIZE = TILE_SHEET_SIZE / 4;

class tileType
{
private:
	std::string name;
	std::string fileName;
	sf::Texture* highTiles;
	sf::Texture* lowTiles;

	static sf::Sprite tempSprite;

	const static sf::IntRect Flat;
	const static sf::IntRect Alternate;
	const static sf::IntRect Top;
	const static sf::IntRect TopLeft;
	const static sf::IntRect TopLeftBot;
	const static sf::IntRect TopLeftRight;
	const static sf::IntRect TopBot;
	const static sf::IntRect TopBotRight;
	const static sf::IntRect TopRight;
	const static sf::IntRect Left;
	const static sf::IntRect LeftBot;
	const static sf::IntRect LeftBotRight;
	const static sf::IntRect LeftRight;
	const static sf::IntRect Bot;
	const static sf::IntRect BotRight;
	const static sf::IntRect Right;

	tileType(void);

public:
	
	tileType(std::string name, std::string fileName);
	tileType(const tileType& other);
	~tileType(void);

	bool loadFromFile(std::string fileName);
	bool loadTile(void);
	bool unloadTile(void);

	std::string getName(void) const;
	std::string getFileName(void) const;

	bool draw(sf::RenderWindow &window, float x, float y, unsigned int height, unsigned int orientation);
};

}