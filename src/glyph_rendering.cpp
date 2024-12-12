#include <cassert>
#include "glyph.h"

#define SCALE 400.f

void Glyph::show_bbox(sf::RenderWindow *window)
{
    float delta_y = float(this->ymax - this->ymin);
    float delta_x = float(this->xmax - this->xmin);

    sf::RectangleShape bbox(sf::Vector2f(delta_x, delta_y));
    bbox.setPosition(this->xmin, this->ymin);
    bbox.setFillColor(sf::Color(32, 32, 32));
    window->draw(bbox);
}

void Glyph::show_glyph(sf::RenderWindow *window)
{
}

void Glyph::show_points(sf::RenderWindow *window)
{
    show_bbox(window);
    sf::Font font;
    if (!font.loadFromFile("/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf"))
    {
        assert(false);
    }
    for (unsigned int i = 0; i < this->vertices.size(); i++)
    {
        struct Vertex vx = vertices[i];
        sf::CircleShape shape(2.f);

        int16_t delta_y = this->ymax - this->ymin;
        int16_t delta_x = this->xmax - this->xmin;

        int16_t glyph_size = (delta_x > delta_y) ? delta_x : delta_y;

        float x_coord = (float(vx.x_coord) / glyph_size) * SCALE + 200.f;
        float y_coord = (float(ymax - vx.y_coord) / glyph_size) * SCALE + 200.f;

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