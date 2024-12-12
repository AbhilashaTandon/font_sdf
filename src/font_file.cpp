#include "font_file.h"

std::uint32_t FontFile::read_32()
{
    std::uint32_t out = 0;

    for (int i = 0; i < 4; i++)
    {
        char byte = 0;
        file.read(&byte, 1);

        out <<= 8;
        out |= (unsigned char)byte;
    }
    return out;
}

uint16_t FontFile::read_16()
{
    uint16_t out = 0;

    for (int i = 0; i < 2; i++)
    {
        char byte = 0;
        file.read(&byte, 1);

        out <<= 8;
        out |= (unsigned char)byte;
    }
    return out;
}

uint8_t FontFile::read_byte()
{
    char byte = 0;
    file.read(&byte, 1);
    return (uint8_t)byte;
}

std::int32_t FontFile::read_32_signed()
{
    std::int32_t out = 0;

    for (int i = 0; i < 4; i++)
    {
        char byte = 0;
        file.read(&byte, 1);

        out <<= 8;
        out |= (unsigned char)byte;
    }
    return out;
}

int16_t FontFile::read_16_signed()
{
    int16_t out = 0;

    for (int i = 0; i < 2; i++)
    {
        char byte = 0;
        file.read(&byte, 1);

        out <<= 8;
        out |= (unsigned char)byte;
    }
    return out;
}

int8_t FontFile::read_byte_signed()
{
    char byte = 0;
    file.read(&byte, 1);
    return (int16_t(0)) | byte;
}

void FontFile::jump_to(const std::uint32_t num_bytes)
{
    file.seekg(num_bytes);
}

void FontFile::skip_ahead(const int32_t offset)
{
    file.seekg(offset, std::ios_base::cur);
}

int FontFile::get_index()
{
    return int(this->file.tellg());
}

FontFile::FontFile(std::string file_path)
{

    file = std::ifstream(file_path, std::ios::in | std::ios::binary);
}

FontFile::~FontFile()
{
    file.close();
}

std::string FontFile::read_string(const uint32_t num_chars)
{
    std::string out = "";

    for (unsigned int i = 0; i < num_chars; i++)
    {
        char next = 0;
        file.read(&next, 1);
        out.push_back(next);
    }
    return out;
}

bool FontFile::good() const
{
    return file.good();
}
