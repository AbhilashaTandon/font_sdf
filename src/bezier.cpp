#include "bezier.h"

std::vector<sf::Vector2f> get_outline(Bezier b)
{

    int delta_x = abs(b.end.x - b.start.x);
    int delta_y = abs(b.end.y - b.start.y);
    int param_range = (delta_x > delta_y) ? delta_x : delta_y;

    std::vector<sf::Vector2f> pts;

    for (int i = 0; i < param_range; i++)
    {
        float t = float(i) / float(param_range);
        sf::Vector2f pt;
        pt.x = b.start.x * (1.f - t) * (1.f - t) + 2.f * b.control.x * (1.f - t) * t + b.end.x * t * t;
        pt.y = b.start.y * (1.f - t) * (1.f - t) + 2.f * b.control.y * (1.f - t) * t + b.end.y * t * t;
        pts.push_back(pt);
    }

    return pts;
}
