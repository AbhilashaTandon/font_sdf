#pragma once
#include <string>
#include <fstream>
#include <cstdint>

class FontFile
{
public:
    std::uint32_t read_32();

    uint16_t read_16();

    uint8_t read_byte();

    std::int32_t read_32_signed();

    int16_t read_16_signed();

    int8_t read_byte_signed();

    void jump_to(const std::uint32_t num_bytes);

    void skip_ahead(const int32_t offset);

    int get_index();

    FontFile(std::string file_path);
    virtual ~FontFile();
    std::string read_string(const uint32_t num_chars);

    bool is_good() const;

private:
    std::ifstream file;
};