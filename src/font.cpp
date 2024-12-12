#include "font.h"
#include <stdexcept>
#include <iostream>

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

    uint16_t version = file.read_16();

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
}

void Font::read_glyphs()
{
}
