#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 resolution;
uniform sampler2D texture;
uniform vec2 position;

void main()
{

    vec2 p = (gl_FragCoord.xy - position) / resolution.xy;

    p.y = 1. - p.y;

    p.y += 0.2;

    p.y *= 1.085;

    vec4 color = vec4(p, 0., 1.);

    // multiply it by the color
    gl_FragColor = gl_Color * color;
}