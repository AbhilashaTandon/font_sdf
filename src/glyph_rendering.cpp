#include <cassert>
#include "glyph.h"

void Glyph::show_glyph(sf::RenderWindow *window)
{
}

void Glyph::show_points(sf::RenderWindow *window)
{
    sf::Font font;
    if (!font.loadFromFile("/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf"))
    {
        assert(false);
    }
    for (int i = 0; i < this->vertices.size(); i++)
    {
        struct Vertex vx = vertices[i];
        sf::CircleShape shape(2.f);

        float x_coord = (float(vx.x_coord) / 16384.f + 1.f) * 200.f + 200.f;
        float y_coord = (float(vx.y_coord) / 16384.f + 1.f) * 200.f + 200.f;

        sf::Text text;

        // select the font
        text.setFont(font); // font is a sf::Font

        // set the string to display
        text.setString(std::to_string(i));

        // set the character size
        text.setCharacterSize(16); // in pixels, not points!

        if (vx.on_curve)
        {
            text.setFillColor(sf::Color::Cyan);
        }
        else
        {
            text.setFillColor(sf::Color::Magenta);
        }
        // set the color
        text.setPosition(sf::Vector2f(x_coord, y_coord));
        window->draw(text);
    }
}