#pragma once
#include "font_file.h"
#include "glyph.h"
#include "unordered_map"

class Font
{
public:
    Font(std::string file_path);

private:
    FontFile file;
    std::string file_path;
    uint16_t num_glyphs;
    uint16_t max_points;
    uint16_t max_contours;
    std::unordered_map<std::string, uint32_t> table_offsets;
    std::unordered_map<std::string, uint32_t> table_lengths;

    std::vector<Glyph> glyphs;
};