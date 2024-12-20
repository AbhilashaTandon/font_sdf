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
    // file_name = "/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf";
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

        f.show_glyph(&window, unicode_value);

        sf::Text unicode_value_display;

        unicode_value_display.setFont(font);

        unicode_value_display.setFillColor(sf::Color::White);

        unicode_value_display.setString(std::to_string(unicode_value));

        unicode_value_display.setCharacterSize(24);

        unicode_value_display.setPosition(30, 30);

        window.draw(unicode_value_display);

        sf::RectangleShape filled_in_glyph;
        filled_in_glyph.setPosition(sf::Vector2f(1000, 185));
        filled_in_glyph.setSize(sf::Vector2f(337, 337));
        filled_in_glyph.setFillColor(sf::Color::White);

        shader.setUniform("resolution", sf::Vector2f(filled_in_glyph.getSize().x, filled_in_glyph.getSize().y));
        shader.setUniform("position", sf::Vector2f(filled_in_glyph.getPosition().x, filled_in_glyph.getPosition().y));

        window.draw(filled_in_glyph, &shader);
        window.display();
    }

    return 0;
}