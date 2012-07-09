#pragma once
#include <vector>
#include "texture.h"
#include <math.h>

namespace iso
{
class animation
{
private:
	std::vector<iso::texture*> textrs;
	unsigned int rate;
	int sinceLast;
	bool loops;
	int curTextr;

public:
	animation(iso::texture* textr, unsigned int msDelay = 0, bool doesLoop = false);
	~animation(void);

	void addTexture(iso::texture* textr);
	void setLoop(bool doesLoop);
	bool getLoop(void) const;
	void setMSDelay(unsigned int msDelay);
	unsigned int getMSDelay(void) const;
	iso::texture* update(unsigned unsigned int msElapsed);
	void reset(void);
};

}