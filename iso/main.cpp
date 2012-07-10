#include <SFML\Graphics.hpp>
#include "textureDict.h"
#include "texture.h"
#include <string>


const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const std::string TEXTURES_FILELIST = "textures\\list.txt";

int main()
{
	
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ISO", sf::Style::Close);
	iso::textureDict textureDB;
	textureDB.loadTextures(TEXTURES_FILELIST);

	// random drawing tests, ignore this stuff
	iso::texture* grass0000 = textureDB.allocTexture("grass_0000"); // flat
	iso::texture* grass1100 = textureDB.allocTexture("grass_1100");
	iso::texture* grass1001 = textureDB.allocTexture("grass_1001");

	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
		grass0000->draw(window, 0, 0);
		grass0000->draw(window, 128, 0);
		grass0000->draw(window, 256, 0);
		grass0000->draw(window, 64, 32);
		grass0000->draw(window, 128+64, 32);
		grass0000->draw(window, 256+64, 32);
		grass0000->draw(window, 128, 64);
		grass1100->draw(window, 256, 96);
		grass1001->draw(window, 64, 128);


        window.display();
    }

    return 0;
}