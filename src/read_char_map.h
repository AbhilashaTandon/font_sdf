#pragma once

#define UNICODE_PLATFORM_ID 0
#define MAC_PLATFORM_ID 1
#define MICROSOFT_PLATFORM_ID 3

#define UNICODE_V1_0 0
#define UNICODE_V1_1 1
#define UNICODE_V2 3

#include "font_file.h"

void read_char_map_table(FontFile *f, uint32_t start_idx, uint32_t length);

void read_formats(FontFile *f);

void read_format_4(FontFile *f);
