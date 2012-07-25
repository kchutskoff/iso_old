#include "texture.h"


iso::texture::texture(std::string file, std::string name, int x, int y, unsigned int w, unsigned int h, bool isAnimated, bool loops)
	:tname(name), animate(isAnimated), rate(0), curFrame(0), dim_w(w), dim_h(w)
{
	// add the first frame to the vector
	frames.push_back(data(x, y, file));
	// we don't keey track of width and height as they should always be the same (file and coordinates can vary though, although file shouldn't as sprite maps should be used).
}

iso::texture::texture(const texture& other)
	:tname(other.tname), animate(other.animate), loops(other.loops), rate(other.rate), sinceLast(other.sinceLast), curFrame(other.curFrame), dim_w(other.dim_w), dim_h(other.dim_h)
{
	frames.resize(other.frames.size());
	for(unsigned int i = 0; i < other.frames.size(); ++i)
	{
		frames[i].fname = other.frames[i].fname;
		frames[i].off_x = other.frames[i].off_x;
		frames[i].off_y = other.frames[i].off_y;
		frames[i].tex = new sf::Texture(*(other.frames[i].tex));
	}
}

iso::texture::~texture(void)
{
	// go through all the frames
	while(frames.size() > 0)
	{
		// if we haven't deallocated, delete
		if(frames[frames.size() -1].tex != NULL)
		{
			delete frames[frames.size() -1].tex;
		}
		frames.pop_back();
	}
}

void iso::texture::setRate(unsigned int MSrate)
{
	rate = MSrate;
}

unsigned int iso::texture::getRate(void) const
{
	return rate;
}

void iso::texture::setLoop(bool isLoop)
{
	loops = isLoop;
}

bool iso::texture::getLoop(void) const{
	return loops;
}

void iso::texture::addFrame(std::string file, int x, int y)
{
	if(animate){
		frames.push_back(data(x, y, file));
	}else{
		// doesn't animate, don't add textures
		// TODO: possible error?
	}
}

unsigned int iso::texture::getAnimationLength(void)
{
	if(!animate)
	{
		return 0;
	}else
	{
		return rate*frames.size();
	}
}

unsigned int iso::texture::getWidth(void) const
{
	return dim_w;
}

unsigned int iso::texture::getHeight(void) const
{
	return dim_h;
}

std::string iso::texture::name(void) const
{
	return tname;
}

void iso::texture::load(void)
{
	for(unsigned int i = 0; i < frames.size(); ++i)
	{
		if(frames[i].tex == NULL)
		{
			frames[i].tex = new sf::Texture();
			frames[i].tex->loadFromFile(frames[i].fname, sf::IntRect(frames[i].off_x, frames[i].off_y, dim_w, dim_h));
		}
	}
}

void iso::texture::unload(void)
{
	for(unsigned int i = 0; i < frames.size(); ++i)
	{
		if(frames[i].tex != NULL){
			delete frames[i].tex;
			frames[i].tex = NULL;
		}	
	}
}

bool iso::texture::draw(sf::RenderWindow &window, float x, float y, unsigned int msElapsed){
	// if we don't animate the texture, leave the curFrame at 0
	if(animate){
		sinceLast += msElapsed;
		// multiple frames could have passed
		while(rate <= sinceLast)
		{
			sinceLast -= rate;
			curFrame++;
		}
		// if we loop, check to see if we need to loop back
		if(loops){
			// again, multiple frames could have passed (even entire animation loops)
			while(curFrame >= frames.size())
			{
				curFrame-=frames.size();
			}
			// don't loop, so if we went over, return a null texture
		}
	}
	
	// check to make sure we haven't finished the animation or the texture isn't ready to draw for some reason
	if((curFrame < frames.size()) && (frames[curFrame].tex != NULL))
	{
		tempSprite.setTexture(*frames[curFrame].tex);
		tempSprite.setPosition(x, y);
		window.draw(tempSprite);
		return true;
	}else{
		// we can't draw it
		return false;
	}
}

void iso::texture::resetAnimation(void)
{
	curFrame = 0;
	sinceLast = 0;
}

sf::Sprite iso::texture::tempSprite;

unsigned int iso::texture::maxTexture = sf::Texture::getMaximumSize();