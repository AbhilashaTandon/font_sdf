#include "bezier.h"

sf::VertexArray get_bezier_indices(sf::Vector2f start, sf::Vector2f anchor, sf::Vector2f end)
{
    int delta_x = abs(end.x - start.x);
    int delta_y = abs(end.y - start.y);
    int param_range = (delta_x > delta_y) ? delta_x : delta_y;

    sf::VertexArray pts(sf::Points, param_range);

    for (int i = 0; i < param_range; i++)
    {
        float t = float(i) / float(param_range);
        sf::Vector2f pt = start * (1 - t) * (1 - t) + 2.f * anchor * (1 - t) * t + end * t * t;
        pts[i].position = pt;
        pts[i].color = sf::Color::White;
    }

    return pts;
}
