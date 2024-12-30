#include "glyph.h"
#include <cassert>

Glyph::Glyph(FontFile *f, uint32_t start_idx, uint16_t units_per_em)
{
    (*f).jump_to(start_idx);
    this->num_contours = (*f).read_16_signed();
    this->compound_glyph = num_contours < 0;
    this->num_contours = compound_glyph ? 0 : num_contours;

    float scale = UNITS_PER_EM / float(units_per_em);

    this->units_per_em = units_per_em;

    this->xmin = (*f).read_16_signed() * scale;
    this->ymin = (*f).read_16_signed() * scale; // negative to invert y axis
    this->xmax = (*f).read_16_signed() * scale;
    this->ymax = (*f).read_16_signed() * scale;

    assert(this->xmin <= this->xmax);
    assert(this->ymin <= this->ymax);

    // these assertions will fail for certain whitespace characters
    this->contour_ends = std::vector<uint16_t>();
    this->contours = std::vector<Contour>();
    this->flags = std::vector<uint8_t>();
    this->num_curves = 0;
    this->num_vertices = 0;
}

void Glyph::read_compound_glyph(FontFile *f)
{
}

constexpr bool is_bit_set(uint8_t byte, uint8_t bit_idx)
{
    return ((byte >> bit_idx) & 1) == 1;
}

// https://stackoverflow.com/a/1180256
// Find the vertex with smallest y (and largest x if there are ties). Let the vertex be A and the previous vertex in the list be B and the next vertex in the list be C. Now compute the sign of the cross product of AB and AC.
// the direction of the points in the contour nominally represents in a font whether its an inner or outer edge, but many fonts don't adhere to this
bool is_clockwise(const std::vector<Vertex> vxs)
{
    struct Vertex lowest = vxs[0];
    int lowest_idx = 0;

    int num_vxs = vxs.size();

    for (int i = 0; i < num_vxs; i++)
    {
        struct Vertex vx = vxs[i];
        if (vx.y < lowest.y)
        {
            lowest = vx;
            lowest_idx = i;
        }
        else if (vx.y == lowest.y)
        {
            if (vx.x > lowest.x)
            {
                lowest = vx;
                lowest_idx = i;
            }
        }
    }
    int prev_vx_idx = (lowest_idx + num_vxs) % (num_vxs + 1); // modulus since we're in a loop
    int next_vx_idx = (lowest_idx + num_vxs + 2) % (num_vxs + 1);

    struct Vertex prev = vxs[prev_vx_idx];
    struct Vertex next = vxs[next_vx_idx];

    int cross = (prev.x - lowest.x) * (next.y - lowest.y) - (prev.y - lowest.y) * (next.x - lowest.x);

    return cross > 0;
}

void Glyph::read_simple_glyph(FontFile *f)
{
    assert(num_contours != 0);

    for (int i = 0; i < num_contours; i++)
    {
        contour_ends.push_back((*f).read_16());
    }

    this->num_vertices = contour_ends.back() + 1;

    assert(num_vertices > 2);

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
    std::vector<int16_t> x_coords = std::vector<int16_t>();
    std::vector<int16_t> y_coords = std::vector<int16_t>();

    x_coords.push_back(0);
    y_coords.push_back(0);
    // first coordinates are relative to 0,0
    // we do this to avoid a special case for the first coordinate

    float scale = UNITS_PER_EM / float(units_per_em);
    // x coords
    for (uint8_t flag : flags)
    {
        bool x_is_byte = is_bit_set(flag, 1);
        // If set, the corresponding x-coordinate is 1 byte long;
        // Otherwise, the corresponding x - coordinate is 2 bytes long

        int16_t last_x_value = x_coords.back();
        int16_t x = 0;
        if (x_is_byte)
        {
            x = (*f).read_byte();                      // implicit cast
            int x_sign = is_bit_set(flag, 4) ? 1 : -1; // this bit describes the sign of the value, with a value of 1 equalling positive and a zero value negative
            x *= x_sign;
        }
        else
        {
            bool x_is_same = is_bit_set(flag, 4);
            // if this bit is set, then the current x-coordinate is the same as the previous x-coordinate
            // if this bit is not set, the current x-coordinate is a signed 16-bit delta vector. In this case, the delta vector is the change in x
            if (!x_is_same)
            {
                x = (*f).read_16_signed();
            }
        }

        x_coords.push_back(last_x_value + x * scale);
    }
    // y coords
    for (uint8_t flag : flags)
    {
        bool y_is_byte = is_bit_set(flag, 2);
        // If set, the corresponding y-coordinate is 1 byte long;
        // Otherwise, the corresponding y - coordinate is 2 bytes long

        int16_t last_y_value = y_coords.back();
        int16_t y = 0;
        if (y_is_byte)
        {
            y = (*f).read_byte();                      // implicit cast
            int y_sign = is_bit_set(flag, 5) ? 1 : -1; // this bit describes the sign of the value, with a value of 1 equalling positive and a zero value negative
            y *= y_sign;
        }
        else
        {
            bool y_is_same = is_bit_set(flag, 5);
            // if this bit is set, then the current y-coordinate is the same as the previous y-coordinate
            // if this bit is not set, the current y-coordinate is a signed 16-bit delta vector. In this case, the delta vector is the change in y
            if (!y_is_same)
            {
                y = (*f).read_16_signed();
            }
        }

        y_coords.push_back(last_y_value + y * scale);
    }

    assert(x_coords.size() == unsigned(this->num_vertices + 1));
    assert(y_coords.size() == unsigned(this->num_vertices + 1));

    int prev_contour_end = -1;
    int contour_end = contour_ends[0];
    for (int contour = 0; contour < num_contours; contour++)
    {
        contour_end = contour_ends[contour];

        struct Contour c;
        std::vector<Vertex> contour_vertices = std::vector<Vertex>();
        for (int vx_idx = prev_contour_end + 1; vx_idx <= contour_end; vx_idx++)
        {
            assert(vx_idx < this->num_vertices);
            uint8_t flag = flags[vx_idx];
            bool on_curve = flag & 1;
            struct Vertex vx;
            vx.x = x_coords[vx_idx + 1];
            vx.y = y_coords[vx_idx + 1];
            vx.vxtype = ((flag & 1) != 0) ? VxType::on_curve : VxType::off_curve;
            contour_vertices.push_back(vx);
        }

        c.is_clockwise = is_clockwise(contour_vertices);

        // add missing vxs on curve btwn off curve pts

        for (unsigned int i = 0; i < contour_vertices.size(); i++)
        {
            struct Vertex current = contour_vertices[i];
            int next_idx = (i + 1 == contour_vertices.size()) ? 0 : (i + 1);

            struct Vertex next = contour_vertices[next_idx];
            if (current.vxtype == off_curve && next.vxtype == off_curve)
            {
                struct Vertex midpoint;
                midpoint.x = (current.x + next.x) / 2;
                midpoint.y = (current.y + next.y) / 2;

                midpoint.vxtype = VxType::hidden;
                contour_vertices.insert(contour_vertices.begin() + i + 1, midpoint);
            }
        }

        int vx_idx = 0;

        // here we take vertices and combine them into bezier splines, containing 2 or 3 pts
        // im not sure if it is allowed by the specification for a contour to begin with an off point curve
        // but if it isn't its still possible some fonts are broken and do it anyways
        // so ill put this in as a safety, we go to the first point on the curve
        while (contour_vertices[vx_idx].vxtype == off_curve)
        {
            vx_idx++;
        }

        int num_contour_vertices = contour_vertices.size();
        int first_vx = vx_idx;

        do
        {
            // we split the vertices into segments of 2 or 3 (with overlap for endpoints)
            assert(contour_vertices[vx_idx].vxtype != off_curve);

            struct Bezier b;
            b.start = contour_vertices[vx_idx];
            vx_idx = ((vx_idx + 1) % num_contour_vertices);

            if (contour_vertices[vx_idx].vxtype != off_curve)
            {
                b.is_curve = false;
                b.end = contour_vertices[vx_idx];
                b.control.x = (b.start.x + b.end.x) / 2;
                b.control.y = (b.start.y + b.end.y) / 2;
            }
            else
            {
                b.is_curve = true;
                b.control = contour_vertices[vx_idx];

                vx_idx = ((vx_idx + 1) % num_contour_vertices);

                b.end = contour_vertices[vx_idx];
            }
            c.curves.push_back(b);

            this->num_curves++;

        } while (vx_idx != first_vx);

        this->contours.push_back(c);

        prev_contour_end = contour_end;
    }
}

void Glyph::convert_vertices(sf::Vector2f pos, float font_size)
{
    for (int i = 0; i < num_contours; i++)
    {
        for (int j = 0; j < contours[i].curves.size(); j++)
        {
            struct Bezier b = contours[i].curves[j];

            b.start = em_to_pixel(b.start, pos, font_size);
            b.control = em_to_pixel(b.control, pos, font_size);
            b.end = em_to_pixel(b.end, pos, font_size);

            contours[i].curves[j] = b;
        }
    }
}

struct Vertex em_to_pixel(struct Vertex vx, sf::Vector2f pos, float font_size)
{
    // converts coordinates in em space (font) to pixel space (window)

    float scale = font_size / (UNITS_PER_EM);
    return Vertex{int16_t(vx.x * scale + pos.x), int16_t(vx.y * scale + pos.y), vx.vxtype};
}

sf::Vector2i em_to_pixel(sf::Vector2i vec, sf::Vector2f pos, float font_size)
{
    // converts coordinates in em space (font) to pixel space (window)

    float scale = font_size / (UNITS_PER_EM);

    return sf::Vector2i(vec.x * scale + pos.x, vec.y * scale + pos.y);
}
