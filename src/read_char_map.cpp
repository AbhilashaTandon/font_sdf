#include "read_char_map.h"
#include "font_file.h"
#include <cassert>

void read_char_map_table(FontFile *f, uint32_t start_idx, uint32_t length)
{
    (*f).jump_to(start_idx);
    uint16_t version = (*f).read_16();
    assert(version == 0);
    uint16_t num_subtables = (*f).read_16();

    uint16_t unicode_2_offset = 0;

    for (int i = 0; i < num_subtables; i++)
    {
        uint16_t platform_id = (*f).read_16();
        uint16_t platform_specific_id = (*f).read_16();
        uint32_t offset = (*f).read_32();
        if ((platform_id == UNICODE_PLATFORM_ID) && (platform_specific_id == UNICODE_V2))
        {
            unicode_2_offset = offset;
            break;
        }
    }

    (*f).jump_to(start_idx + unicode_2_offset);

    read_formats(f);
}

void read_formats(FontFile *f)
{
    for (int i = 0; i < 100; i++)
    {

        uint16_t format = (*f).read_16();
        printf("format %d\n", format);

        // only formats 4 and 12 are really used, but we still need to handle their haders to skip ahead to the next one
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
            read_format_4(f);
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
}

void read_format_4(FontFile *f)
{
    uint16_t reserved = (*f).read_16();
    assert(reserved == 0);
    uint32_t length = (*f).read_32();
    uint32_t language = (*f).read_32(); // unneeded for my purposes
    uint32_t num_groups = (*f).read_32();

    for (int i = 0; i < num_groups; i++)
    {
        uint32_t first_char_code = (*f).read_32();
        uint32_t last_char_code = (*f).read_32();
        uint32_t start_glyph_code = (*f).read_32();
        printf("%d\t%d\t%d\n", first_char_code, last_char_code, start_glyph_code);
    }
}
