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
    
    p.x = (gl_FragCoord.x - position.x) / 1000;
    p.y = (window_size.y - gl_FragCoord.y - position.y) / 1000;

    vec4 color = vec4(0.);

    ivec2 p_ems = ivec2((p + vec2(-0.1, -.9)) * vec2(2000.));


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

        if(length(start_coords - p_ems) < 10.){
            color.r = 1.;
        }

        if(length(control_coords - p_ems) < 10.){
            color.g = 1.;
        }

        if(length(end_coords- p_ems) < 10.){
            color.b = 1.;
        }

    }
    color.a = 1.;


    // multiply it by the color
    gl_FragColor = color; 
}