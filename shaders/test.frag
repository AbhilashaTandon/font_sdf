#version 330
uniform int num_curves;
uniform sampler2D beziers;

uniform float font_size;
uniform float units_per_em;
uniform vec2 position;
uniform vec2 size;

vec2 convert_coordinate(vec2 v){
    return (v - position) * units_per_em / font_size;
}

void main()
{
    int num_vertices = num_curves * 3;
    vec2 coords = convert_coordinate(gl_FragCoord.zw) / 1024.;
   

    // multiply it by the color
    gl_FragColor = vec4(coords, 0., 1.); 

}