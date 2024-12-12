#include "glyph.h"
#include <cassert>

Glyph::Glyph(FontFile *f, uint32_t start_idx)
{
    (*f).jump_to(start_idx);
    this->num_contours = (*f).read_16_signed();
    this->compound_glyph = num_contours < 0;
    this->xmin = (*f).read_16_signed();
    this->ymin = (*f).read_16_signed();
    this->xmax = (*f).read_16_signed();
    this->ymax = (*f).read_16_signed();
    assert(this->xmin <= this->xmax);
    assert(this->ymin <= this->ymax);
}

Glyph::Glyph(FontFile *f)
{
    int current_idx = (*f).get_index();
    printf("\n\nindex: %d 0x%x\n\n", (*f).get_index(), (*f).get_index());
    this->num_contours = (*f).read_16_signed();
    this->compound_glyph = num_contours < 0;
    this->xmin = (*f).read_16_signed();
    this->ymin = (*f).read_16_signed();
    this->xmax = (*f).read_16_signed();
    this->ymax = (*f).read_16_signed();
    assert(this->xmin <= this->xmax);
    assert(this->ymin <= this->ymax);
}

void Glyph::read_compound_glyph(FontFile *f)
{
}

bool is_bit_set(uint8_t byte, uint8_t bit_idx)
{
    return ((byte >> bit_idx) & 1) == 1;
}

void Glyph::read_simple_glyph(FontFile *f)
{
    assert(num_contours != 0);

    for (int i = 0; i < num_contours; i++)
    {
        contour_ends.push_back((*f).read_16());
    }

    this->num_vertices = contour_ends.back() + 1;

    uint16_t instruction_len = (*f).read_16();
    (*f).skip_ahead(instruction_len);

    int flag_count = 0;
    while (flag_count < this->num_vertices)
    {
        char flag = (*f).read_byte();
        assert(!is_bit_set(flag, 6) && !is_bit_set(flag, 7));
        if (is_bit_set(flag, 3))
        {
            // if 3rd bit is set then next byte is # of additional repeats of flag
            char repeats = (*f).read_byte();
            for (int i = 0; i < repeats + 1; i++)
            {
                flag_count++;
                this->flags.push_back(flag);
            }
        }
        else
        {
            flag_count++;
            this->flags.push_back(flag);
        }
    }
    assert(flag_count == this->num_vertices);
    assert(flags.size() == this->num_vertices);
    std::vector<int16_t> x_coords{};
    std::vector<int16_t> y_coords{};

    x_coords.push_back(0);
    y_coords.push_back(0);
    // first coordinates are relative to 0,0
    // we do this to avoid a special case for the first coordinate

    // x coords
    for (uint8_t flag : flags)
    {
        bool x_is_byte = is_bit_set(flag, 1);
        // If set, the corresponding x-coordinate is 1 byte long;
        // Otherwise, the corresponding x - coordinate is 2 bytes long

        int16_t last_x_value = x_coords.back();
        int16_t x_coord = 0;
        if (x_is_byte)
        {
            x_coord = (*f).read_byte();                // implicit cast
            int x_sign = is_bit_set(flag, 4) ? 1 : -1; // this bit describes the sign of the value, with a value of 1 equalling positive and a zero value negative
            x_coord *= x_sign;
        }
        else
        {
            bool x_is_same = is_bit_set(flag, 4);
            // if this bit is set, then the current x-coordinate is the same as the previous x-coordinate
            // if this bit is not set, the current x-coordinate is a signed 16-bit delta vector. In this case, the delta vector is the change in x
            if (!x_is_same)
            {
                x_coord = (*f).read_16_signed();
            }
        }

        x_coords.push_back(last_x_value + x_coord);
    }
    // y coords
    for (uint8_t flag : flags)
    {
        bool y_is_byte = is_bit_set(flag, 2);
        // If set, the corresponding y-coordinate is 1 byte long;
        // Otherwise, the corresponding y - coordinate is 2 bytes long

        int16_t last_y_value = y_coords.back();
        int16_t y_coord = 0;
        if (y_is_byte)
        {
            y_coord = (*f).read_byte();                // implicit cast
            int y_sign = is_bit_set(flag, 5) ? 1 : -1; // this bit describes the sign of the value, with a value of 1 equalling positive and a zero value negative
            y_coord *= y_sign;
        }
        else
        {
            bool y_is_same = is_bit_set(flag, 5);
            // if this bit is set, then the current y-coordinate is the same as the previous y-coordinate
            // if this bit is not set, the current y-coordinate is a signed 16-bit delta vector. In this case, the delta vector is the change in y
            if (!y_is_same)
            {
                y_coord = (*f).read_16_signed();
            }
        }

        y_coords.push_back(last_y_value + y_coord);
    }

    assert(x_coords.size() == unsigned(this->num_vertices + 1));
    assert(y_coords.size() == unsigned(this->num_vertices + 1));
    for (int i = 1; i < this->num_vertices + 1; i++)
    {
        uint8_t flag = flags[i - 1];
        bool on_curve = flag & 1;
        struct Vertex v{x_coords[i], y_coords[i], on_curve};
        this->vertices.push_back(v);
        bool x_short = is_bit_set(flag, 1);
        bool y_short = is_bit_set(flag, 2);
        bool x_sign = x_short ? (is_bit_set(flag, 4)) : x_coords[i] > 0;
        bool y_sign = y_short ? (is_bit_set(flag, 5)) : y_coords[i] > 0;
        // printf("%d %d %s %s %s %s %s\n", x_coords[i], y_coords[i], on_curve ? "ON_CURVE" : "OFF_CURVE", x_short ? "X 1 byte" : "X 2 bytes", y_short ? "Y 1 byte" : "Y 2 bytes", x_sign ? "X POS" : "X NEG", y_sign ? "Y POS" : "Y NEG");
    }
}
