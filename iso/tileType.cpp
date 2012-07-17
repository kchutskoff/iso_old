#include "tileType.h"


iso::tileType::tileType(void)
	:name(""), fileName(""), highTiles(NULL), lowTiles(NULL)
{
}

iso::tileType::tileType(std::string name, std::string fileName)
	:name(name), fileName(fileName)
{
	highTiles = new sf::Texture();
	highTiles->loadFromFile(fileName, sf::IntRect(0, 0, TILE_SHEET_SIZE, TILE_SHEET_SIZE));
	lowTiles = new sf::Texture();
	lowTiles->loadFromFile(fileName, sf::IntRect(0, TILE_SHEET_SIZE, TILE_SHEET_SIZE, TILE_SHEET_SIZE));
}

iso::tileType::tileType(const tileType& other)
	:name(other.name), fileName(other.fileName)
{
	if(other.highTiles != NULL)
	{
		highTiles = new sf::Texture(*other.highTiles);
	}else
	{
		highTiles = NULL;
	}
	if(other.lowTiles != NULL)
	{
		lowTiles = new sf::Texture(*other.lowTiles);
	}else
	{
		lowTiles = NULL;
	}
}

iso::tileType::~tileType(void)
{
	if(highTiles != NULL)
	{
		delete highTiles;
	}
	if(lowTiles != NULL)
	{
		delete lowTiles;
	}
}

std::string iso::tileType::getName(void) const
{
	return name;
}
std::string iso::tileType::getFileName(void) const
{
	return fileName;
}

bool iso::tileType::loadTile(void)
{
	if(highTiles != NULL && lowTiles != NULL)
	{
		highTiles = new sf::Texture();
		highTiles->loadFromFile(fileName, sf::IntRect(0, 0, TILE_SHEET_SIZE, TILE_SHEET_SIZE));

		lowTiles = new sf::Texture();
		lowTiles->loadFromFile(fileName, sf::IntRect(0, TILE_SHEET_SIZE, TILE_SHEET_SIZE, TILE_SHEET_SIZE));

		return true;
	}
	return false;
}

bool iso::tileType::unloadTile(void)
{
	if(highTiles != NULL && lowTiles != NULL)
	{
		delete highTiles;
		highTiles = NULL;
		delete lowTiles;
		lowTiles = NULL;
		return true;
	}
	return false;
}

bool iso::tileType::draw(sf::RenderWindow &window, float x, float y, unsigned int height, unsigned int orientation)
{
	if(height >= 1024)
	{
		// TODO: invalid height
		return false;
	}
	if(height % 2 == 0)
	{
		// even, use high tiles
		tempSprite.setTexture(*highTiles);
		// TLBR
		switch(orientation)
		{
		case 0x0: tempSprite.setTextureRect(Flat); break;
		case 0x1: tempSprite.setTextureRect(Right); break;
		case 0x2: tempSprite.setTextureRect(Bot); break;
		case 0x3: tempSprite.setTextureRect(BotRight); break;
		case 0x4: tempSprite.setTextureRect(Left); break;
		case 0x5: tempSprite.setTextureRect(LeftRight); break;
		case 0x6: tempSprite.setTextureRect(LeftBot); break;
		case 0x7: tempSprite.setTextureRect(LeftBotRight); break;
		case 0x8: tempSprite.setTextureRect(Top); break;
		case 0x9: tempSprite.setTextureRect(TopRight); break;
		case 0xA: tempSprite.setTextureRect(TopBot); break;
		case 0xB: tempSprite.setTextureRect(TopBotRight); break;
		case 0xC: tempSprite.setTextureRect(TopLeft); break;
		case 0xD: tempSprite.setTextureRect(TopLeftRight); break;
		case 0xE: tempSprite.setTextureRect(TopLeftBot); break;
		case 0xF: tempSprite.setTextureRect(Alternate); break;
		default: return false; break; // TODO: error bad orientation
		}
		tempSprite.setPosition(x, y);
		window.draw(tempSprite);
		return true;
	}
}

const sf::IntRect iso::tileType::Flat = sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::Alternate = sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::Top = sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::TopLeft = sf::IntRect(0, TILE_SIZE*2, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::TopLeftBot = sf::IntRect(TILE_SIZE*2, TILE_SIZE*3, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::TopLeftRight = sf::IntRect(TILE_SIZE, TILE_SIZE*3, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::TopBot = sf::IntRect(TILE_SIZE*2, 0, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::TopBotRight = sf::IntRect(0, TILE_SIZE*3, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::TopRight = sf::IntRect(TILE_SIZE, TILE_SIZE*2, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::Left = sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::LeftBot = sf::IntRect(TILE_SIZE*2, TILE_SIZE*2, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::LeftBotRight = sf::IntRect(TILE_SIZE*3, TILE_SIZE*3, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::LeftRight = sf::IntRect(TILE_SIZE*3, 0, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::Bot = sf::IntRect(TILE_SIZE*3, TILE_SIZE, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::BotRight = sf::IntRect(TILE_SIZE*3, TILE_SIZE*2, TILE_SIZE, TILE_SIZE);
const sf::IntRect iso::tileType::Right = sf::IntRect(TILE_SIZE*2, TILE_SIZE, TILE_SIZE, TILE_SIZE);

sf::Sprite iso::tileType::tempSprite;