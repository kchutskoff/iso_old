#pragma once
#include "texture.h"


iso::texture::texture(std::string file, std::string name, int x, int y, unsigned int w, unsigned int h)
{
	fname = file;
	tname = name;
	off_x = x;
	off_y = y;
	dim_w = w;
	dim_h = h;
	tex = NULL;
}

iso::texture::~texture(void)
{
	fname = "";
	tname = "";
	if(tex != NULL)
	{
		delete tex;
		tex = NULL;
	}
}

int iso::texture::x(void) const
{
	return off_x;
}

int iso::texture::y(void) const
{
	return off_y;
}

unsigned int iso::texture::w(void) const
{
	return dim_w;
}

unsigned int iso::texture::h(void) const
{
	return dim_h;
}

std::string iso::texture::file(void) const
{
	return fname;
}

std::string iso::texture::name(void) const
{
	return tname;
}

sf::Texture* iso::texture::textr(void) const
{
	return tex;
}

void iso::texture::loadTexture(void)
{
	if(tex == NULL)
	{
		tex = new sf::Texture();
		tex->loadFromFile(fname, sf::IntRect(off_x, off_y, dim_w, dim_h));
	}else{
		// TODO: error double load
	}
}

void iso::texture::unloadTexture(void)
{
	if(tex != NULL)
	{
		delete tex;
		tex = NULL;
	}else{
		// TODO: error double unload
	}
}

void iso::texture::draw(sf::RenderWindow &window, float x, float y){
	tempSprite.setTexture(*tex);
	tempSprite.setPosition(x, y);
	window.draw(tempSprite);
}

sf::Sprite iso::texture::tempSprite;