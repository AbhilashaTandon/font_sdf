#include <iostream>
#include <fstream>
#include <cassert>
#include "font_file.h"
#include "glyph.h"
#include "font.h"
#include <SFML/Graphics.hpp>

int main()
{
    // Font f = Font();
    std::string file_name = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    file_name = "/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf";
    Font f = Font(file_name);
    sf::RenderWindow window(sf::VideoMode(1600, 800), "SFML works!");

    sf::Font font;
    if (!font.loadFromFile(file_name))
    {
        return 1;
        // lazy error handling
    }

    uint32_t unicode_value = 33;

    while (f.get_glyph_offset(unicode_value) == -1)
    {
        unicode_value++;
    }

    bool color_contours = false;

    sf::Shader shader;

    if (!shader.loadFromFile("../shaders/shader.frag", sf::Shader::Fragment))
    {
        return -1;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::Escape)
                {
                    window.close();
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Right)
                {
                    unicode_value++;

                    while (f.get_glyph_offset(unicode_value) == -1)
                    {
                        unicode_value++;
                    }
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Left)
                {
                    unicode_value--;
                    while (f.get_glyph_offset(unicode_value) == -1)
                    {
                        unicode_value--;
                    }
                }

                else if (event.key.scancode == sf::Keyboard::Scan::Space)
                {
                    color_contours = !color_contours;
                }
            }
        }
        window.clear(sf::Color(64, 48, 32));

        f.show_glyph(&window, unicode_value, &shader);

        window.display();
    }

    return 0;
}