#version 330
uniform int num_curves;
uniform vec2 resolution;
uniform sampler2D beziers;
uniform vec2 position;
uniform vec2 window_size;


void main()
{
    int num_vertices = num_curves * 3;
    vec2 p;
    
    p.x = (gl_FragCoord.x - position.x) / resolution.x;
    p.y = (window_size.y - gl_FragCoord.y - position.y) / resolution.y;

    // multiply it by the color
    gl_FragColor = vec4(p, 0., 1.); 

}