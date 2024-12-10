#include <iostream>
#include <fstream>
#include <cassert>
#include "font_file.h"
#include "read_char_map.h"
#include "glyph.h"

#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");

    uint32_t cmap_start = 0;
    uint32_t glyf_start = 0;
    uint32_t cmap_len = 0;
    uint32_t glyf_len = 0;

    FontFile f = FontFile("/home/abhilasha/Fonts/FiraCodeNerdFont-Medium.ttf");
    if (f.good())
    {

        std::uint32_t scaler_type = f.read_32();
        printf("scaler type 0x%08x\n", scaler_type);

        uint16_t numTables = f.read_16();
        uint16_t searchRange = f.read_16();
        uint16_t entrySelector = f.read_16();
        uint16_t rangeShift = f.read_16();

        printf("num tables %u\n", numTables);
        printf("search range %u\n", searchRange);
        printf("entry selector %u\n", entrySelector);
        printf("range shift %u\n", rangeShift);

        printf("table name, checksum, offset, length\n");
        for (int i = 0; i < numTables; i++)
        {
            std::string table_name = f.read_string(4);
            uint32_t check_sum = f.read_32();
            uint32_t offset = f.read_32();
            uint32_t length = f.read_32();

            if (table_name == "cmap")
            {
                cmap_start = offset;
                cmap_len = length;
            }
            else if (table_name == "glyf")
            {
                glyf_start = offset;
                glyf_len = length;
            }

            std::cout << table_name << ", ";
            printf("%u, ", check_sum);
            printf("%u, ", offset);
            printf("%u\n", length);
        }
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        Glyph g = Glyph(&f, glyf_start);
        g.read_glyph(&f);
        g.show_points(&window);

        window.display();
    }

    return 0;
}