#include "animation.h"


iso::animation::animation(iso::texture* textr, unsigned int msDelay = 0, bool doesLoop = false)
{	
	iso::texture* temp;
	rate = msDelay;
	loops = doesLoop;
	curTextr = 0;
	sinceLast = 0;

	textrs.push_back(textr);
}

iso::animation::~animation(void)
{
}

void iso::animation::addTexture(iso::texture* textr)
{
	if (textr != NULL)
	{
		textrs.push_back(textr);
	}else{
		// TODO: error null texture
	}
}

void iso::animation::setLoop(bool doesLoop){

}

bool iso::animation::getLoop(void) const{

}

void iso::animation::setMSDelay(unsigned int msDelay)
{
	rate = msDelay;
}

unsigned int iso::animation::getMSDelay(void) const
{
	return rate;
}

iso::texture* iso::animation::update(unsigned int msElapsed)
{
	sinceLast += msElapsed;
	// multiple frames could have passed
	while(rate <= sinceLast)
	{
		sinceLast -= rate;
		curTextr++;
	}
	// if we loop, check to make sure we don't go back
	if(loops){
		// again, multiple frames could have passed (even entire animation loops)
		while(curTextr >= textrs.size())
		{
			curTextr-=textrs.size();
		}
		return textrs[curTextr];
		// don't loop, so if we went over, return a null texture
	}else{
		if(curTextr >= textrs.size()
			){
			return NULL;
		}else
		{
			return textrs[curTextr];
		}
	}
}

void iso::animation::reset(void){
	curTextr = 0;
	sinceLast = 0;
}




}
