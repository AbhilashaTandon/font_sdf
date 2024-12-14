#include <iostream>
#include <fstream>
#include <cassert>
#include "font_file.h"
#include "glyph.h"
#include "font.h"
#include <SFML/Graphics.hpp>

int main()
{
    // Font f = Font("/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf");
    Font f = Font("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    sf::RenderWindow window(sf::VideoMode(1600, 800), "SFML works!");

    uint32_t ascii_code = 0;

    while (f.get_glyph_offset(ascii_code) == -1)
    {
        ascii_code++;
    }

    bool color_contours = false;

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
                    ascii_code++;

                    while (f.get_glyph_offset(ascii_code) == -1)
                    {
                        ascii_code++;
                    }
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Left)
                {
                    ascii_code--;
                    while (f.get_glyph_offset(ascii_code) == -1)
                    {
                        ascii_code--;
                    }
                }

                else if (event.key.scancode == sf::Keyboard::Scan::Space)
                {
                    color_contours = !color_contours;
                }
            }
        }
        window.clear();

        f.show_points(&window, ascii_code, color_contours);
        window.display();
    }

    return 0;
}