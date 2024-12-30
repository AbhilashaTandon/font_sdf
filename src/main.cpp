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
    float font_size = 400.f;

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

    sf::Vector2f pos = sf::Vector2f(1200.f, 200.f);

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
                else if (event.key.scancode == sf::Keyboard::D)
                {
                    unicode_value++;

                    while (f.get_glyph_offset(unicode_value) == -1)
                    {
                        unicode_value++;
                    }
                }
                else if (event.key.scancode == sf::Keyboard::A)
                {
                    unicode_value--;
                    while (f.get_glyph_offset(unicode_value) == -1)
                    {
                        unicode_value--;
                    }
                }

                else if (event.key.scancode == sf::Keyboard::Scan::W)
                {
                    font_size *= 1.01;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::S)
                {
                    font_size /= 1.01;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Space)
                {
                    color_contours = !color_contours;
                }

                else if (event.key.scancode == sf::Keyboard::Scan::Left)
                {
                    pos.x -= 10.f;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Right)
                {
                    pos.x += 10.f;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Up)
                {
                    pos.y -= 10.f;
                }
                else if (event.key.scancode == sf::Keyboard::Scan::Down)
                {
                    pos.y += 10.f;
                }
            }
        }
        window.clear(sf::Color(64, 48, 32));

        f.show_glyph_debug(&window, unicode_value, pos, font_size, &shader);

        window.display();
    }

    return 0;
}