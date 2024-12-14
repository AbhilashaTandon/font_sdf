#include "font.h"

std::vector<cmap_range> read_formats(FontFile *f)
{
    for (int i = 0; i < 100; i++)
    {

        uint16_t format = (*f).read_16();

        // only formats 4 and 12 are really used, but we still need to handle their headers to skip ahead to the next one
        switch (format)
        {
        case 0:
        case 2:
        case 4:
        case 6:
        {
            uint16_t length = (*f).read_16();
            (*f).skip_ahead(length - 4); //-4 because we already read 4 bytes
            break;
        }
        case 8:
        case 10:
        case 12:
            return read_format_12(f);
        case 13:
        {
            (*f).skip_ahead(2);
            uint32_t length = (*f).read_32();
            (*f).skip_ahead(length - 8);
            break;
        }
        case 14:
        {
            uint32_t length = (*f).read_32();
            (*f).skip_ahead(length - 6);
            break;
        }
        default:
            i = 100;
            break;
        }
    }

    throw std::runtime_error("CMAP table format 12 not found");
}

std::vector<cmap_range> read_format_12(FontFile *f)
{
    uint16_t reserved = (*f).read_16();
    assert(reserved == 0);
    uint32_t length = (*f).read_32();
    (*f).read_32(); // language: unneeded for my purposes
    uint32_t num_groups = (*f).read_32();

    std::vector<cmap_range> ranges = std::vector<cmap_range>();

    for (int i = 0; i < num_groups; i++)
    {
        uint32_t first_char_code = (*f).read_32();
        uint32_t last_char_code = (*f).read_32();
        uint32_t start_glyph_code = (*f).read_32();

        if (first_char_code >= 128)
        {
            break; // past ascii printable range
        }

        struct cmap_range current_range{first_char_code, last_char_code, start_glyph_code};
        ranges.push_back(current_range);
    }

    return ranges;
}

void Font::read_cmap()
{
    uint32_t cmap_start = this->table_offsets["cmap"];
    file.jump_to(cmap_start);
    uint16_t version = file.read_16();
    assert(version == 0);
    uint16_t num_subtables = file.read_16();

    uint16_t unicode_2_offset = 0;

    for (int i = 0; i < num_subtables; i++)
    {
        uint16_t platform_id = file.read_16();
        uint16_t platform_specific_id = file.read_16();
        uint32_t offset = file.read_32();
        if ((platform_id == UNICODE_PLATFORM_ID) && (platform_specific_id == UNICODE_V2))
        {
            // we're only interested in the Unicode V2 cmap table
            unicode_2_offset = offset;
            break;
        }
    }

    file.jump_to(cmap_start + unicode_2_offset);

    this->cmap_ranges = read_formats(&file);
}