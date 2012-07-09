#pragma once
#include <map>
#include <string>
#include "animation.h"

namespace iso
{

class sprite
{
public:
	std::map<std::string, iso::animation*> animations;

	void addAnimation(iso::animation*, std::string name);

	void playAnimation(std::string name);

	sprite(void);
	~sprite(void);
};

}