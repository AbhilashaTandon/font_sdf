#include "bezier.h"

sf::VertexArray get_outline(Bezier b, sf::Vector2f pos)

{
    int delta_x = abs(b.end.x - b.start.x);
    int delta_y = abs(b.end.y - b.start.y);
    int param_range = (delta_x > delta_y) ? delta_x : delta_y;

    if (b.is_curve)
    {
        sf::VertexArray curve(sf::LineStrip, param_range);

        for (int i = 0; i < param_range; i++)
        {
            float t = float(i) / float(param_range);
            sf::Vector2f vx;
            vx.x = b.start.x * (1.f - t) * (1.f - t) + 2.f * b.control.x * (1.f - t) * t + b.end.x * t * t;
            vx.y = b.start.y * (1.f - t) * (1.f - t) + 2.f * b.control.y * (1.f - t) * t + b.end.y * t * t;
            sf::Vertex v;
            v.position = vx + pos;
            v.color = sf::Color::White;

            curve[i] = v;
        }

        return curve;
    }
    else
    {
        sf::VertexArray line(sf::Lines, 2);

        line[0].color = sf::Color::White;
        line[1].color = sf::Color::White;

        line[0].position = sf::Vector2f(b.start.x, b.start.y) + pos;
        line[1].position = sf::Vector2f(b.end.x, b.end.y) + pos;

        return line;
    }
}
