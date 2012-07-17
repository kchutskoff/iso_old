#pragma once
#include <string>
#include <vector>
#include <SFML\Graphics.hpp>

namespace iso{

class texture
{
private:
	std::string tname;
	bool animate;
	bool loops;
	unsigned int rate;
	unsigned int sinceLast;
	unsigned int curFrame;
	unsigned int dim_w;
	unsigned int dim_h;

	struct data
	{
	public:
		// not storing width and height, as they should be the same for all textures
		data(int x, int y, std::string fname)
			:off_x(x), off_y(y), fname(fname), tex(NULL) {};
		data(void)
			:off_x(0), off_y(0), fname(""), tex(NULL){};

		int off_x;
		int off_y;
		std::string fname;
		sf::Texture* tex;
	};

	std::vector<data> frames;

	static sf::Sprite tempSprite;

	texture(void);

public:
	texture(std::string file, std::string name, int x, int y, unsigned int w, unsigned int h, bool isAnimated = false, bool isLoop = true);
	texture(const texture& other);
	~texture(void);	

	sf::Texture* textr(void) const;

	void setRate(unsigned int MSrate);
	unsigned int getRate(void) const;

	void setLoop(bool isLoop);
	bool getLoop(void) const;

	unsigned int getAnimationLength(void);

	unsigned int width(void) const;
	unsigned int height(void) const;

	std::string name(void) const;

	void addFrame(std::string file, int x, int y);
	
	bool iso::texture::draw(sf::RenderWindow &window, float x, float y, unsigned int msElapsed = 0);

	void resetAnimation(void);

	void loadTexture(void);
	void unloadTexture(void);
};

}