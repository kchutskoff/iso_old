#include <SFML\Graphics.hpp>
#include "textureDict.h"
#include "texture.h"
#include <string>
#include "worldMap.h"
#include <sstream>
#include "tileType.h"
#include "PhysFS++.h"


const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const std::string TEXTURES_FILELIST = "textures\\list.txt";

int main()
{
	
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ISO", sf::Style::Close);
	iso::textureDict textureDB;
	textureDB.loadTextureList(TEXTURES_FILELIST);

	// random drawing tests, ignore this stuff
	iso::texture* grass0000 = textureDB.allocTexture("grass_0000"); // flat
	iso::texture* grass1100 = textureDB.allocTexture("grass_1100");
	iso::texture* grass1001 = textureDB.allocTexture("grass_1001");
	iso::texture* grass1111 = textureDB.allocTexture("grass_1111"); // alt

	sf::Text myText;

	iso::worldMap test = iso::worldMap("grass", iso::worldMap::size(32, 32));
	test.writeToFile("testmap.map");
	test.loadFromFile("testmap.map");

	sf::Clock tempClock;
	sf::Time tempTime = tempClock.getElapsedTime();
	sf::Int64 sinceLast = tempTime.asMicroseconds();

	sf::Texture testTexture;
	testTexture.loadFromFile("textures\\environment\\grass.png", sf::IntRect(0, 0, 128, 128));
	sf::Sprite testSprite;
	testSprite.setTexture(testTexture);

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
		tempTime = tempClock.getElapsedTime();
		sf::Int64 curTime = tempTime.asMicroseconds();
		sf::Int64 uSince = curTime - sinceLast;
		sinceLast = curTime;

        window.clear();
		float fps = 0;
		if(uSince != 0)
		{
			fps = 1000000/float(uSince);
		}
		std::stringstream tempStream;
		tempStream << fps << "  " << sf::Texture::getMaximumSize();
		myText.setString(tempStream.str());

		for(int x = 0; x < 1000; x++)
		{
			testSprite.setPosition(float(rand()%800), float(rand()%600));
			window.draw(testSprite);
		}

		window.draw(myText);


        window.display();
    }

    return 0;
}