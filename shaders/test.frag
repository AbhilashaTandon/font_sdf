#version 330
uniform int num_curves;
uniform sampler2D beziers;

uniform float font_size;
uniform float units_per_em;
uniform vec2 position;
uniform float bbox_height;

vec2 pixel_to_em(vec2 px){
    float scale = units_per_em / bbox_height;
    return (px - position) * scale; 
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
        ivec2 start_coords;
        start_coords.x = start_texel.r * 256 + start_texel.g;
        start_coords.y = (start_texel.b * 256 + start_texel.a);
      
        ivec2 control_coords;
        control_coords.x = control_texel.r * 256 + control_texel.g;
        control_coords.y = (control_texel.b * 256 + control_texel.a);

        ivec2 end_coords;
        end_coords.x = end_texel.r * 256 + end_texel.g;
        end_coords.y = (end_texel.b * 256 + end_texel.a);

        start_coords.x -= start_coords.x > 32768 ? 65536 : 0;
        start_coords.y = start_coords.y > 32768 ? 65536 - start_coords.y : -start_coords.y;

        control_coords.x -= control_coords.x > 32768 ? 65536 : 0;
        control_coords.y = control_coords.y > 32768 ? 65536 - control_coords.y : -control_coords.y;

        end_coords.x -= end_coords.x > 32768 ? 65536 : 0;
        end_coords.y = end_coords.y > 32768 ? 65536 - end_coords.y : -end_coords.y;

        //TODO: fix this by making inputs unsigned (add 32768 to the signed value)
       
        if(length(start_coords - coords) < 10.){
            color.r = 1.;
        } 
 
        if(length(control_coords - coords) < 10.){
            color.g = 1.;
        } 
 
        if(length(end_coords - coords) < 10.){
            color.b= 1.;
        } 
    }

    color.a = 1.;

    gl_FragColor = color; 
}