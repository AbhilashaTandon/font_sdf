#include <stdexcept>
#include <iostream>
#include "font.h"

const std::string req_tables[] = {"cmap", "glyf", "head", "hhea", "hmtx", "loca", "maxp", "name", "post"};

Font::Font(std::string file_path) : file(file_path)
{
    this->file_path = file_path;
    if (!this->file.is_good())
    {
        throw std::runtime_error("Font file could not be opened.");
    }

    std::uint32_t scaler_type = file.read_32();

    uint16_t numTables = file.read_16();
    uint16_t searchRange = file.read_16();
    uint16_t entrySelector = file.read_16();
    uint16_t rangeShift = file.read_16();

    for (int i = 0; i < numTables; i++)
    {
        std::string table_name = file.read_string(4);
        file.read_32(); // checksum not needed
        uint32_t offset = file.read_32();
        uint32_t length = file.read_32();

        this->table_offsets[table_name] = offset;
        this->table_lengths[table_name] = length;
    }

    for (std::string table_name : req_tables)
    {
        if (this->table_offsets.find(table_name) == table_offsets.end())
        {
            std::cerr << "Could not find table " << table_name << " in file.";
            throw std::runtime_error("");
        }
    }

    file.jump_to(table_offsets["maxp"]);

    uint32_t version = file.read_32();

    if (version == 0x00005000)
    {
        throw std::runtime_error("Font file is a PostScript open type font.");
    }

    else if (version != 0x00010000)
    {
        throw std::runtime_error("Font file has invalid format");
    }

    this->num_glyphs = file.read_16();
    this->max_points = file.read_16();
    this->max_contours = file.read_16();

    file.jump_to(this->table_offsets["head"] + 36);
    this->xmin = file.read_16_signed();
    this->ymin = file.read_16_signed();
    this->xmax = file.read_16_signed();
    this->ymax = file.read_16_signed();

    assert(this->xmax >= this->xmin);
    assert(this->ymax >= this->ymin);

    this->aspect_ratio = float(this->xmax - this->xmin) / float(this->ymax - this->ymin);

    file.skip_ahead(6);

    switch (file.read_16_signed())
    {
    case 0:
        this->long_glyph_offsets = false;
        break;
    case 1:
        this->long_glyph_offsets = true;
        break;
    default:
        throw std::runtime_error("Malformed head table");
    }

    read_cmap();
    read_loca();
}

void Font::read_loca()
{
    uint32_t loca_start = this->table_offsets["loca"];
    uint32_t loca_length = this->table_lengths["loca"];
    file.jump_to(loca_start);

    if (this->long_glyph_offsets)
    {
        uint32_t max_iter = loca_length / 4;
        for (unsigned int i = 0; i < max_iter; i++)
        {
            this->glyph_offsets.push_back(file.read_32());
        }
    }
    else
    {
        uint32_t max_iter = loca_length / 2;
        for (unsigned int i = 0; i < max_iter; i++)
        {
            this->glyph_offsets.push_back(uint32_t(file.read_16()));
        }
    }

    // if long_glyph_offsets all loca entries are uint32s, else theyre uint16s
}

int Font::get_glyph_offset(uint32_t unicode_value) const
{

    for (unsigned int i = 0; i < cmap_ranges.size(); i++)
    {
        struct cmap_range range = cmap_ranges[i];
        if (range.first_char_code > unicode_value)
        {
            continue;
        }
        uint32_t glyph_id = range.start_glyph_id + (unicode_value - range.first_char_code);
        return (glyph_id < glyph_offsets.size()) ? glyph_offsets[glyph_id] : -1;
    }
    return -1;
}

Glyph Font::get_glyph_outline(uint32_t unicode_value)
{
    auto glyph_loc = glyphs.find(unicode_value);

    if (glyph_loc == glyphs.end())
    // if not cached yet
    {
        int offset = get_glyph_offset(unicode_value);
        printf("cache miss: %d %d\n", unicode_value, offset);
        if (offset == -1)
        {
            throw std::runtime_error("Glyph not found in file");
        }
        Glyph g = Glyph(&file, this->table_offsets["glyf"] + offset);

        g.read_glyph(&file);
        glyphs.insert({unicode_value, g});
        return g;
    }
    else
    {

        printf("cache hit: %d %d\n", unicode_value, glyph_loc->first);
        return glyph_loc->second;
    }
}
