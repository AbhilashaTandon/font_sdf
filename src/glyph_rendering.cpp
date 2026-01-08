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

void Font::show_glyph_debug(sf::RenderWindow *window, uint32_t char_code, sf::Vector2f pos, float font_size, sf::Shader *shader)
{

    const float glyph_top = em_to_pixel(sf::Vector2i(0, UNITS_PER_EM), pos, font_size).y;
    // height of glyph (1024 ems) in pixel space
    Glyph g = get_glyph_outline(char_code);

    render_glyph(window, g, pos, font_size, shader);
    g.convert_vertices(sf::Vector2f(0., 0.), font_size);

    draw_ref_glyph(window, char_code, pos, font_size); // uses the builtin sfml render to compare to ours
    assert(g.contours.size() > 0);
    for (struct Contour c : g.contours)
    {
        assert(c.curves.size() > 0);
        for (struct Bezier b : c.curves)
        {
            sf::VertexArray pts = get_outline(b, sf::Vector2f(600.f, pos.y + glyph_top));

            // we do this to reference the top left (0, -1024) of the glyph instead of the bottom left(0, 0)
            // thats more intuitive

            // convert_vertices(pts, sf::Vector2f(600.f, pos.y), font_size);
            window->draw(pts);
        }
    }

    display_char_code(window, char_code);
}

float Font::render_glyph(sf::RenderWindow *window, Glyph g, sf::Vector2f pos, float font_size, sf::Shader *shader)
{
    float window_height = window->getSize().y * UNITS_PER_EM / font_size;
    // pos is top left of glyph
    const sf::Vector2f bbox_bottom_left = sf::Vector2f(em_to_pixel(sf::Vector2i(0, UNITS_PER_EM), pos, font_size));
    // const sf::Vector2f bbox_top_left = sf::Vector2f(em_to_pixel(sf::Vector2i(0, 0), pos, font_size));
    // box with dimensions 1em x 1em with bottom at baseline and left at left edge

    // on baseline at left
    sf::Vector2i char_top_left = em_to_pixel(sf::Vector2i(g.xmin, UNITS_PER_EM - g.ymax), pos, font_size);

    sf::Vector2i char_bottom_right = em_to_pixel(sf::Vector2i(g.xmax, UNITS_PER_EM - g.ymin), pos, font_size);

    sf::Vector2i char_bbox_size = char_bottom_right - char_top_left;

    assert(char_bottom_right.y >= char_top_left.y);

    sf::RectangleShape char_bbox(sf::Vector2f(char_bbox_size.x, char_bbox_size.y));
    char_bbox.setPosition(sf::Vector2f(char_top_left));

    sf::Uint8 *vertices = new sf::Uint8[g.num_curves * 3 * 4]; // * 4 because pixels have 4 components (RGBA)
    // the r and g components are used for the higher and lower bytes of the x coordinate respectively
    // and likewise for b and a for the y coordinate

    int idx = 0;

    for (struct Contour contour : g.contours)
    {
        for (struct Bezier bezier : contour.curves)
        {
            // pack coordinates into color channels
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

    shader->setUniform("font_size", font_size);
    shader->setUniform("position", sf::Vector2f(bbox_bottom_left.x, window->getSize().y - bbox_bottom_left.y)); // since gl_FragCoord has y axis invert i need to flip all coordinates i pass to the shader
    shader->setUniform("units_per_em", UNITS_PER_EM);

    window->draw(char_bbox, shader);

    return char_bottom_right.x - char_top_left.x;
}

void Font::draw_ref_glyph(sf::RenderWindow *window, uint32_t char_code, sf::Vector2f pos, float font_size)
{
    sf::Text ref_glyph;
    sf::Font current_font;
    current_font.loadFromFile(this->file_path);
    ref_glyph.setFont(current_font);
    std::string unicode_char = "";
    unicode_char += (char(char_code));
    ref_glyph.setString(unicode_char);
    ref_glyph.setPosition(sf::Vector2f(50, pos.y));
    ref_glyph.setCharacterSize(int(font_size));
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

float Font::show_glyph(sf::RenderWindow *window, uint32_t char_code, sf::Vector2f pos, float font_size, sf::Shader *shader)
{
    Glyph g = get_glyph_outline(char_code);

    return render_glyph(window, g, pos, font_size, shader);
}

void Font::render_text(sf::RenderWindow *window, std::string text, sf::Vector2f pos, float font_size, sf::Shader *shader, float padding)
{
    for (char c : text)
    {
        if (c == ' ')
        {
            pos += sf::Vector2f(font_size / 2.5 + padding * font_size / UNITS_PER_EM, 0.);
            // add space width
            // font_size is 1 em width in pixels
            continue;
        }
        float right_edge = show_glyph(window, uint32_t(c), pos, font_size, shader);
        pos += sf::Vector2f(right_edge + padding * font_size / UNITS_PER_EM, 0.);
    }
}
