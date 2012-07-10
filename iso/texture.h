#pragma once
#include <string>
#include <SFML\Graphics.hpp>

namespace iso{

class texture
{
private:
	int off_x;
	int off_y;
	unsigned int dim_w;
	unsigned int dim_h;
	std::string fname;
	std::string tname;
	sf::Texture* tex;

	static sf::Sprite tempSprite;

public:
	texture(std::string file, std::string name, int x, int y, unsigned int w, unsigned int h);
	~texture(void);	

	sf::Texture* textr(void) const;

	int x(void) const;
	int y(void) const;
	unsigned int w(void) const;
	unsigned int h(void) const;
	std::string file(void) const;
	std::string name(void) const;
	void iso::texture::draw(sf::RenderWindow &window, float x, float y);

protected:
	void loadTexture(void);
	void unloadTexture(void);
	friend class textureDict;
};

}