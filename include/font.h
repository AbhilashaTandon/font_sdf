#pragma once

#define MAC_PLATFORM_ID 1
#define MICROSOFT_PLATFORM_ID 3
#define UNICODE_PLATFORM_ID 0
#define UNICODE_V1_0 0
#define UNICODE_V1_1 1
#define UNICODE_V2 3

#include "font_file.h"
#include "glyph.h"
#include <unordered_map>
#include <vector>
#include <cassert>

struct cmap_range
{
    uint32_t first_char_code;
    uint32_t last_char_code;
    uint32_t start_glyph_id;
};
// each range is a sequential group of mappings
// first_char_code -> start_glyph_id
// first_char_code + 1 -> start_glyph_id + 1
//...
// last_char_code -> start_glyph_id + (last_char_code - first_char_code)

class Font
{
public:
    Font(std::string file_path);

    void read_cmap();
    void read_loca();

    int get_glyph_offset(uint32_t unicode_value) const;

    Glyph get_glyph_outline(uint32_t unicode_value);

    // display methods
    void show_bbox(sf::RenderWindow *window, Glyph g, sf::Vector2f pos, float font_size);
    void show_glyph(sf::RenderWindow *window, uint32_t char_code, sf::Vector2f pos, float font_size, sf::Shader *shader);
    void render_glyph(sf::RenderWindow *window, Glyph g, sf::Vector2f pos, float font_size, sf::Shader *shader);
    void draw_ref_glyph(sf::RenderWindow *window, uint32_t char_code, sf::Vector2f pos, float font_size);
    void display_char_code(sf::RenderWindow *window, uint32_t char_code);

    void show_points(sf::RenderWindow *window, uint32_t char_code, bool color_contours);

private:
    FontFile file;
    std::string file_path;
    uint16_t num_glyphs;
    uint16_t max_points;
    uint16_t max_contours;
    std::unordered_map<std::string, uint32_t> table_offsets;
    std::unordered_map<std::string, uint32_t> table_lengths;

    std::unordered_map<uint32_t, Glyph> glyphs; // map from char codes to glyphs

    bool long_glyph_offsets;
    // whether glyph offsets in loca table are 2 bytes (false) or 4 bytes (true)

    std::vector<cmap_range> cmap_ranges;
    std::vector<uint32_t> glyph_offsets;

    int16_t xmin;
    int16_t xmax;
    int16_t ymin;
    int16_t ymax;

    float aspect_ratio;

    uint16_t units_per_em;
};

// i want some way of mapping ascii codes to glyphs
// the best way to do this probably is a map from char codes to glyphs
// the way this is stored in the file is as follows
// the cmap table maps character codes to glyph ids
// the loca table maps glyph ids to offsets in the glyf table
// because of this i will need some intermediate representation of the data in cmap
std::vector<cmap_range> read_formats(FontFile *f);
std::vector<cmap_range> read_format_12(FontFile *f);