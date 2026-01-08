#pragma once
#include "font_file.h"
#include "glyph.h"
#include "font.h"
#include <SFML/Graphics.hpp>

void render_text(sf::RenderWindow &window, uint32_t &unicode_value, Font &f, float &font_size, sf::Vector2f &pos, std::string &text, sf::Shader &shader);
