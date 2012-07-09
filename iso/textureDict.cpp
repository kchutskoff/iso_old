#pragma once
#include "textureDict.h"


iso::textureDict::textureDict(void)
{
}


iso::textureDict::~textureDict(void)
{
}


bool iso::textureDict::loadTextures(std::string filelist)
{
	texture* temp = NULL;
	bool error = false;
	errno_t err;
	FILE* f;
	char tname[128] = "";
	char fname[128] = "";
	std::string tname_str = "";
	std::string fname_str = "";
	int x = 0;
	int y = 0;
	unsigned int w = 0;
	unsigned int h = 0;

	if((err = fopen_s(&f, filelist.c_str(), "r") != 0))
	{
		// TODO: ERROR file not found
		return false;
	}

	while(!feof(f)){
		error = false;
		// read in values
		if(fscanf_s(f, " %s", &tname, 128) <= 0)
		{
			error = true;
		}
		if(error || fscanf_s(f, " %s", &fname, 128) <= 0)
		{
			error = true;
		}
		if(error || fscanf_s(f, " %d", &x) <= 0)
		{
			error = true;
		}
		if(error || fscanf_s(f, " %d", &y) <= 0)
		{
			error = true;
		}
		if(error || fscanf_s(f, " %u", &w) <= 0)
		{
			error = true;
		}
		if(error || fscanf_s(f, " %u", &h) <= 0)
		{
			error = true;
		}
		if(error){
			// TODO: deal with error
		}else{
			tname_str = tname;
			fname_str = fname;
			// make a new texture
			temp = new texture(fname, tname, x, y, w, h);
			if(dict.count(tname) > 0){
				// TODO: error, texture already exists
			}else{
				// add the texture to the dictionary under its texture name, with a ref count of 0
				dict.insert( std::pair<std::string, std::pair<int, iso::texture*> >(tname, std::pair<int, iso::texture*>(0, temp) ) );
			}
		}
		
	}
	

	return true;
}

iso::texture* iso::textureDict::allocTexture(std::string name)
{
	std::map<std::string, std::pair<int, texture*>>::iterator i = dict.find(name);
	if(i == dict.cend())
	{
		// not found
		return NULL;

		// TODO: ERROR missing texture
	}
	
	// check if initialized
	if(i->second.first == 0){
		// load the texture to memory
		i->second.second->loadTexture();
	}

	// increase the reference count
	i->second.first++;

	return i->second.second;
}

bool iso::textureDict::releaseTexture(texture* t)
{
	std::map<std::string, std::pair<int, texture*>>::iterator i = dict.find(t->name());
	if(i == dict.cend())
	{
		// not found
		return false;

		// TODO: ERROR double_free
	}
	// decrease the reference count
	i->second.first--;

	// check if unreferenced
	if(i->second.first == 0){
		i->second.second->unloadTexture();
	}
	return false;
}