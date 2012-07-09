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
	iso::texture* grass1 = textureDB.allocTexture("grass1");
	iso::texture* grass11 = textureDB.allocTexture("grass1");
    sf::Text text("Hello SFML");
	sf::Sprite myGrass;
	myGrass.setTexture(*grass1->textr(), false);
	myGrass.setPosition(64, 64);
	myGrass.setScale(2, 2);
	myGrass.setRotation(25);

	


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
		window.draw(myGrass);
        window.draw(text);
		
        window.display();
    }

    return 0;
}