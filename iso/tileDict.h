#pragma once
#include <string>
#include "tileType.h"

namespace iso
{

class tileDict
{
public:
	tileDict(void);
	~tileDict(void);

	tileType* allocateTile(std::string tileName);
	bool releaseTile(tileType* tile);
};

}