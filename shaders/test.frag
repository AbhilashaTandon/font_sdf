#version 330
uniform int num_curves;
uniform sampler2D beziers;

uniform float font_size;
uniform float units_per_em;
uniform vec2 position;

vec2 pixel_to_em(vec2 px){
    float scale = units_per_em / font_size;
    return (px - position) * scale; 
}

void main()
{
    int num_vertices = num_curves * 3;
    // vec2 coords = pixel_to_em(gl_FragCoord.xy) ; 
    vec2 coords = (gl_FragCoord.xy - position) / font_size;
    
    gl_FragColor = vec4(coords, 0., 1.); 
}