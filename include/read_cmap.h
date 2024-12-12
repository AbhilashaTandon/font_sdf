#pragma once

#define UNICODE_PLATFORM_ID 0
#define MAC_PLATFORM_ID 1
#define MICROSOFT_PLATFORM_ID 3

#define UNICODE_V1_0 0
#define UNICODE_V1_1 1
#define UNICODE_V2 3

#include "font_file.h"

#include "font.h"
#include <vector>

std::vector<cmap_range> read_cmap_table(FontFile *f, uint32_t start_idx, uint32_t length);
std::vector<cmap_range> read_formats(FontFile *f);
std::vector<cmap_range> read_format_12(FontFile *f);
