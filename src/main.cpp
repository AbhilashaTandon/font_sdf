#include <iostream>
#include <fstream>
#include <cassert>
#include "font_file.h"
#include "glyph.h"
#include "font.h"
#include <SFML/Graphics.hpp>

int main()
{

    Font f = Font("/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf");

    // Glyph g = Glyph(&f, glyf_start);
    // g.read_glyph(&f);

    // sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");

    // while (window.isOpen())
    // {
    //     sf::Event event;
    //     while (window.pollEvent(event))
    //     {
    //         if (event.type == sf::Event::Closed)
    //         {
    //             window.close();
    //         }
    //         else if (event.type == sf::Event::KeyPressed)
    //         {
    //             if (event.key.scancode == sf::Keyboard::Scan::Escape)
    //             {
    //                 window.close();
    //             }
    //             else if (event.key.scancode == sf::Keyboard::Scan::Space)
    //             {
    //                 g = Glyph(&f);
    //                 g.read_glyph(&f);
    //             }
    //         }
    //     }

    //     window.clear();
    //     g.show_points(&window);

    //     window.display();
    // }

    return 0;
}