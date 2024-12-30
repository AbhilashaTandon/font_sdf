
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

ivec2 convert_coords(ivec4 texel){
    ivec2 coords;

    //unpack coordinates
    coords.x = texel.r * 256 + texel.g;
    coords.y = texel.b * 256 + texel.a;

    coords.x -= coords.x > 32768 ? 65536 : 0;
    coords.y -= coords.y > 32768 ? 65536 : 0;

    return coords;
}

void main()
{
    int num_vertices = num_curves * 3;
    vec2 coords = pixel_to_em(gl_FragCoord.xy) ; 
    
    vec4 color = vec4(0.);

    for(int i = 0; i < num_vertices; i+=3){
        ivec4 start_texel = ivec4(texelFetch(beziers, ivec2(i, 0), 0) * 255.);
        ivec4 control_texel = ivec4(texelFetch(beziers, ivec2(i + 1, 0), 0) * 255.);
        ivec4 end_texel = ivec4(texelFetch(beziers, ivec2(i + 2, 0), 0) * 255.);

        //unpack coordinates 
        ivec2 start_coords = convert_coords(start_texel);
        ivec2 control_coords = convert_coords(control_texel);
        ivec2 end_coords = convert_coords(end_texel);

        if(length(start_coords - coords) < 5.){
            color.r = 1.;
        } 
 
        if(length(control_coords - coords) < 5.){
            color.g = 1.;
        } 
 
        if(length(end_coords - coords) < 5.){
            color.b= 1.;
        } 
    }

    color.a = 1.;

    gl_FragColor = color; 
}