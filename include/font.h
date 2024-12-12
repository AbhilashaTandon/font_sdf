#pragma once
#include "font_file.h"
#include "glyph.h"
#include "unordered_map"

class Font
{
public:
    Font(std::string file_path);

    void read_glyphs();

private:
    FontFile file;
    std::string file_path;
    uint16_t num_glyphs;
    uint16_t max_points;
    uint16_t max_contours;
    std::unordered_map<std::string, uint32_t> table_offsets;
    std::unordered_map<std::string, uint32_t> table_lengths;

    std::unordered_map<uint16_t, Glyph> glyphs; // map from char codes to glyphs

    bool long_glyph_offsets;
    // whether glyph offsets in loca table are 2 bytes (false) or 4 bytes (true)
};

// i want some way of mapping ascii codes to glyphs
// the best way to do this probably is a map from char codes to glyphs
// the way this is stored in the file is as follows
// the cmap table maps character codes to glyph ids
// the loca table maps glyph ids to offsets in the glyf table
// because of this i will need some intermediate representation of the data in cmap

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