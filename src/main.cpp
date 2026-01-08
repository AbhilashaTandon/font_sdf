#include <iostream>
#include <fstream>
#include <cassert>
#include "main.h"

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
    float font_size = 12.f;

    uint32_t unicode_value = 33;

    while (f.get_glyph_offset(unicode_value) == -1)
    {
        unicode_value++;
    }

    sf::Shader shader;
    if (!shader.loadFromFile("../shaders/sdf.frag", sf::Shader::Fragment))
    {
        return -1;
    }

    sf::Vector2f pos = sf::Vector2f(0.f, 0.f);

    std::string text = "The quick brown fox jumps over the lazy dog. 1234567890 !@#$%^&*() -";

    render_text(window, unicode_value, f, font_size, pos, text, shader);

    return 0;
}

void render_text(sf::RenderWindow &window, uint32_t &unicode_value, Font &f, float &font_size, sf::Vector2f &pos, std::string &text, sf::Shader &shader)
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)

            {
                window.close();
            }
            if (event.type != sf::Event::KeyPressed)
            {
                continue;
            }
            switch (event.key.scancode)
            {
            case sf::Keyboard::Scan::Escape:
                window.close();
                break;
            case sf::Keyboard::Scan::D:
                // next char
                unicode_value++;

                while (f.get_glyph_offset(unicode_value) == -1)
                {
                    unicode_value++;
                }
                break;
            case sf::Keyboard::Scan::A:
                unicode_value--;
                while (f.get_glyph_offset(unicode_value) == -1)
                {
                    unicode_value--;
                }
                break;
            case sf::Keyboard::Scan::W:
                font_size *= 1.01;
                break;
            case sf::Keyboard::Scan::S:
                font_size /= 1.01;
                break;
            case sf::Keyboard::Scan::Left:
                pos.x -= 10.f;
                break;
            case sf::Keyboard::Scan::Right:
                pos.x += 10.f;
                break;
            case sf::Keyboard::Scan::Up:
                pos.y -= 10.f;
                break;
            case sf::Keyboard::Scan::Down:
                pos.y += 10.f;
                break;
            default:
                break;
            }
        }

        window.clear(sf::Color(48, 64, 72));

        // f.show_glyph_debug(&window, unicode_value, pos, font_size, &shader);

        f.render_text(&window, text, pos, font_size, &shader, 200.f);

        window.display();
    }
}