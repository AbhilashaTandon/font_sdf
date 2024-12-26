#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "font_file.h"
#include <iterator>
#include "bezier.h"
struct Contour
{
    std::vector<Bezier> curves;
    bool is_clockwise;
};

struct Vertex convert_coordinate(struct Vertex vx, sf::Vector2f pos, float font_size);
sf::Vector2i convert_coordinate(sf::Vector2i vec, sf::Vector2f pos, float font_size);

class Glyph
{
    friend class Font;

public:
    Glyph(FontFile *f, uint32_t start_idx, uint16_t units_per_em);
    void read_compound_glyph(FontFile *f);
    void read_simple_glyph(FontFile *f);
    void convert_vertices(sf::Vector2f pos, float font_size);
    // we always pass in file to make it clear to the caller when the FontFile char pointer is being moved

    inline void read_glyph(FontFile *f)
    {
        if (compound_glyph)
        {
            read_compound_glyph(f);
        }
        else
        {
            read_simple_glyph(f);
        }
    }

private:
    int16_t num_contours;
    bool compound_glyph;
    int16_t xmin;
    int16_t xmax;
    int16_t ymin;
    int16_t ymax;
    std::vector<uint8_t> flags;
    uint16_t num_vertices;
    uint16_t num_curves;
    std::vector<uint16_t> contour_ends; /// len is num_contours
    std::vector<Contour> contours;

    uint16_t units_per_em;
};
