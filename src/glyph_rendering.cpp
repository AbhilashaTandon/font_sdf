#include <cassert>
#include "font.h"
#include "bezier.h"
#define PADDING 64.f
#define TEXT_SIZE 16
#define BBOX_COLOR sf::Color(32, 32, 32)
#define CHAR_BBOX_COLOR sf::Color(64, 64, 64)

const sf::Color contour_colors[9] = {
    sf::Color(14, 168, 255),
    sf::Color(180, 55, 255),
    sf::Color(145, 66, 0),
    sf::Color(255, 156, 15),
    sf::Color(161, 249, 153),
    sf::Color(0, 236, 241),
    sf::Color(0, 66, 10),
    sf::Color(0, 150, 0),
    sf::Color(45, 93, 158)};

sf::Vector2f Font::convert_coordinate(sf::Vector2f vec, const sf::RenderWindow *window) const
{
    // converts coordinates in em space (font) to pixel space (window)

    sf::Vector2u window_size = window->getSize();

    float view_width = (float(window_size.x) - 2 * PADDING);
    float view_height = (float(window_size.y) - 2 * PADDING);

    float window_aspect_ratio = view_width / view_height;

    float delta_x = float(this->xmax - this->xmin);
    float delta_y = float(this->ymax - this->ymin);

    assert(delta_x > 0);
    assert(delta_y > 0);

    float x_ratio = view_width / delta_x;
    float y_ratio = view_height / delta_y;

    float scale_factor = (x_ratio > y_ratio) ? y_ratio : x_ratio;

    assert(scale_factor > 0);

    int window_x = ((vec.x - this->xmin) * scale_factor + PADDING);
    int window_y = view_height + PADDING - ((vec.y - this->ymin) * scale_factor + PADDING);

    //

    assert(window_x >= PADDING);
    assert(window_y >= PADDING);
    assert(window_x <= window_size.x - PADDING);
    assert(window_y <= window_size.y - PADDING);

    // we make these unsigned ints to force them to align with pixel grid
    return sf::Vector2f(window_x, window_y);
}

sf::VertexArray Font::convert_vertices(sf::VertexArray &va, const sf::RenderWindow *window) const
{
    for (size_t i = 0; i < va.getVertexCount(); i++)
    {
        va[i].position = convert_coordinate(va[i].position, window);
    }
    return va;
}

void Font::show_bbox(sf::RenderWindow *window, uint32_t char_code)
{

    sf::Vector2u window_size = window->getSize();
    sf::Vector2f top_left = convert_coordinate(sf::Vector2f(this->xmin, this->ymin), window);
    sf::Vector2f bottom_right = convert_coordinate(sf::Vector2f(this->xmax, this->ymax), window);

    sf::Vector2f bbox_size = bottom_right - top_left;

    sf::RectangleShape bbox(bbox_size);

    assert(top_left.x >= PADDING);
    assert(top_left.y >= PADDING);
    assert(bottom_right.x <= float(window_size.x) - PADDING);
    assert(bottom_right.y <= float(window_size.y) - PADDING);

    bbox.setPosition(top_left.x, top_left.y);
    bbox.setFillColor(BBOX_COLOR);
    window->draw(bbox);
}

void Font::show_glyph(sf::RenderWindow *window, uint32_t char_code)
{

    // show_bbox(window, char_code);
    Glyph g = get_glyph_outline(char_code);

    sf::Vector2f char_top_left = convert_coordinate(sf::Vector2f(g.xmin, g.ymin), window);

    sf::Vector2f char_bottom_right = convert_coordinate(sf::Vector2f(g.xmax, g.ymax), window);

    sf::Vector2f char_bbox_size = char_bottom_right - char_top_left;

    sf::RectangleShape char_bbox(char_bbox_size - 3.f * sf::Vector2f(-TEXT_SIZE, TEXT_SIZE));
    char_bbox.setPosition(char_top_left.x - TEXT_SIZE, char_top_left.y + 2.f * TEXT_SIZE);

    // // lazy way of adding padding

    char_bbox.setFillColor(CHAR_BBOX_COLOR);
    window->draw(char_bbox);

    sf::Font font;
    if (!font.loadFromFile(this->file_path))
    {
        assert(false);
    }

    sf::Text ref_glyph;
    sf::Font current_font;
    current_font.loadFromFile(this->file_path);
    ref_glyph.setFont(current_font);
    std::string unicode_char = "";
    unicode_char += (char(char_code));
    ref_glyph.setString(unicode_char);
    ref_glyph.setPosition(sf::Vector2f(0, 84));
    ref_glyph.setCharacterSize(int(410));
    ref_glyph.setFillColor(sf::Color::White);
    window->draw(ref_glyph);

    assert(g.contours.size() > 0);
    for (struct Contour c : g.contours)
    {
        assert(c.curves.size() > 0);
        for (struct Bezier b : c.curves)
        {
            sf::VertexArray pts = get_outline(b);
            convert_vertices(pts, window);
            window->draw(pts);
        }
    }
}