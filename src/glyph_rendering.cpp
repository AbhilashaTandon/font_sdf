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

void Font::show_bbox(sf::RenderWindow *window, Glyph g)
{
    sf::Vector2f char_top_left = convert_coordinate(sf::Vector2f(g.xmin, g.ymin), window);

    sf::Vector2f char_bottom_right = convert_coordinate(sf::Vector2f(g.xmax, g.ymax), window);

    sf::Vector2f char_bbox_size = char_bottom_right - char_top_left;

    sf::RectangleShape char_bbox(char_bbox_size - 3.f * sf::Vector2f(-TEXT_SIZE, TEXT_SIZE));
    char_bbox.setPosition(char_top_left.x - TEXT_SIZE, char_top_left.y + 2.f * TEXT_SIZE);

    // // lazy way of adding padding

    char_bbox.setFillColor(CHAR_BBOX_COLOR);
    window->draw(char_bbox);
}

void Font::show_glyph(sf::RenderWindow *window, uint32_t char_code, sf::Shader *shader)
{

    Glyph g = get_glyph_outline(char_code);

    show_bbox(window, g);
    draw_ref_glyph(window, char_code); // uses the builtin sfml render to compare to ours
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

    display_char_code(window, char_code);

    render_glyph(window, g, shader);
}

void Font::render_glyph(sf::RenderWindow *window, Glyph g, sf::Shader *shader)
{
    sf::Uint8 *vertices = new sf::Uint8[g.num_curves * 3 * 4]; // * 4 because pixels have 4 components (RGBA)
    // the r and g components are used for the higher and lower bytes of the x coordinate respectively
    // and likewise for b and a for the y coordinate

    int idx = 0;

    for (struct Contour contour : g.contours)
    {
        for (struct Bezier bezier : contour.curves)
        {
            vertices[idx++] = ((bezier.start.x) & 0xff00) >> 8;
            // higher order byte of x coord
            vertices[idx++] = (bezier.start.x) & 0x00ff;
            // lower order byte of x coord
            vertices[idx++] = ((bezier.start.y) & 0xff00) >> 8;
            // higher order byte of y coord
            vertices[idx++] = (bezier.start.y) & 0x00ff;
            // lower order byte of y coord

            vertices[idx++] = ((bezier.control.x) & 0xff00) >> 8;
            // higher order byte of x coord
            vertices[idx++] = (bezier.control.x) & 0x00ff;
            // lower order byte of x coord
            vertices[idx++] = ((bezier.control.y) & 0xff00) >> 8;
            // higher order byte of y coord
            vertices[idx++] = (bezier.control.y) & 0x00ff;
            // lower order byte of y coord

            vertices[idx++] = ((bezier.end.x) & 0xff00) >> 8;
            // higher order byte of x coord
            vertices[idx++] = (bezier.end.x) & 0x00ff;
            // lower order byte of x coord
            vertices[idx++] = ((bezier.end.y) & 0xff00) >> 8;
            // higher order byte of y coord
            vertices[idx++] = (bezier.end.y) & 0x00ff;
            // lower order byte of y coord
        }
    }

    sf::Texture curves;

    if (!curves.create(g.num_curves * 3, 1))
    {
        // error..
    }

    curves.setRepeated(false);

    curves.update(vertices);

    shader->setUniform("beziers", curves);

    shader->setUniform("num_curves", (int)g.num_curves);

    sf::RectangleShape filled_in_glyph;
    filled_in_glyph.setPosition(sf::Vector2f(1200, 185));
    filled_in_glyph.setSize(sf::Vector2f((g.xmax - g.xmin), g.ymax - g.ymin));
    filled_in_glyph.setFillColor(sf::Color::White);

    shader->setUniform("resolution", sf::Vector2f(filled_in_glyph.getSize().x, filled_in_glyph.getSize().y));
    shader->setUniform("position", sf::Vector2f(filled_in_glyph.getPosition().x, filled_in_glyph.getPosition().y));
    shader->setUniform("window_size", sf::Vector2f(window->getSize()));

    window->draw(filled_in_glyph, shader);
}

void Font::draw_ref_glyph(sf::RenderWindow *window, uint32_t char_code)
{
    sf::Text ref_glyph;
    sf::Font current_font;
    current_font.loadFromFile(this->file_path);
    ref_glyph.setFont(current_font);
    std::string unicode_char = "";
    unicode_char += (char(char_code));
    ref_glyph.setString(unicode_char);
    ref_glyph.setPosition(sf::Vector2f(50, 84));
    ref_glyph.setCharacterSize(int(410));
    ref_glyph.setFillColor(sf::Color::White);
    window->draw(ref_glyph);
}

void Font::display_char_code(sf::RenderWindow *window, uint32_t char_code)
{
    sf::Font font;
    if (!font.loadFromFile(this->file_path))
    {
        assert(false);
    }

    sf::Text unicode_value_display;

    unicode_value_display.setFont(font);

    unicode_value_display.setFillColor(sf::Color::White);

    unicode_value_display.setString(std::to_string(char_code));

    unicode_value_display.setCharacterSize(24);

    unicode_value_display.setPosition(100, 30);

    window->draw(unicode_value_display);
}