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

vec2 convert_coords(ivec4 texel){
    vec2 coords;

    //unpack coordinates
    coords.x = texel.r * 256. + texel.g;
    coords.y = texel.b * 256. + texel.a;

    //fix sign
    coords.x -= coords.x > 32768. ? 65536. : 0.;
    coords.y -= coords.y > 32768. ? 65536. : 0.;

    return coords;
}

void main()
{
    int num_vertices = num_curves * 3;
    vec2 coords = pixel_to_em(gl_FragCoord.xy) ; 
    
    vec4 color = vec4(0.);

    int winding_number = 0;

    for(int i = 0; i < num_vertices; i+=3){
        ivec4 start_texel = ivec4(texelFetch(beziers, ivec2(i, 0), 0) * 255.);
        ivec4 control_texel = ivec4(texelFetch(beziers, ivec2(i + 1, 0), 0) * 255.);
        ivec4 end_texel = ivec4(texelFetch(beziers, ivec2(i + 2, 0), 0) * 255.);

        vec2 start_coords = convert_coords(start_texel) - coords;
        vec2 control_coords = convert_coords(control_texel) - coords;
        vec2 end_coords = convert_coords(end_texel) - coords;


        if(length(start_coords) < 5.){
            color.r = 1.;
            color.a = 1.;
        } 
 
        if(length(control_coords) < 5.){
            color.g = 1.;
            color.a = 1.;
        } 
 
        if(length(end_coords) < 5.){
            color.b = 1.;
            color.a = 1.;
        }

        
        uint code = ((start_coords.y > 0.) ? 2U : 0U) + ((control_coords.y > 0.) ? 4U : 0U) + ((end_coords.y > 0.) ? 8U : 0U); 

        if(code != 0U){
            vec2 a = start_coords - 2. * control_coords + end_coords;
            vec2 b = start_coords - control_coords;
            float ra = 1. / a.y;

            float d = sqrt(max(b.y * b.y - a.y * start_coords.y, 0.0));
            float t1 = (b.y - d) * ra;
            float t2 = (b.y + d) * ra;

            if(abs(a.y) < .001){
                t1 = t2 = start_coords.y * 0.5 / b.y;
            }

            float x1 = (a.x * t1 - b.x * 2.) * t1 + start_coords.x;
            float x2 = (a.x * t2 - b.x * 2.) * t2 + start_coords.x;
            
            winding_number += int(step(0., x1));
            winding_number -= int(step(0., x2));

        } 

    }

    color = vec4(winding_number * vec3(0.1), 1.);

    gl_FragColor = color; 
}