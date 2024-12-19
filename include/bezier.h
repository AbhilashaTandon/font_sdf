#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum VxType
{
    off_curve,
    on_curve,
    hidden, // ttf files dont encode certain redundant points on curve between two helper points
    // so we add them back in here
};
struct Vertex
{
    int32_t x;
    int32_t y;
    VxType vxtype;
};

struct Bezier
{
    struct Vertex start;
    struct Vertex end;
    struct Vertex control; // if line segment we ignore this
    bool is_curve;         // if true quadratic bezier, false is 2 pt line segment
};

// TODO: make bezier class w 2 or 3 vertices, move the above struct and enum to bezier.h
// change contour to list of beziers

bool is_intersecting(struct Bezier b, int32_t x_value);
// not anti-aliased

std::vector<struct Vertex> get_outline(struct Bezier b);