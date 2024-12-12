#include "font.h"

Font::Font(std::string file_path)
{
    this->file_path = file_path;
    this->file = FontFile(file_path);
}