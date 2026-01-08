
#version 330

uniform int num_curves;
uniform sampler2D beziers;

uniform float font_size;
uniform float units_per_em;
uniform vec2 position;

float scale = units_per_em / font_size;

vec2 pixel_to_em(vec2 px){
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

float epsilon = 0.0001;
int MAX_ITER = 20;

void main()
{
    int num_vertices = num_curves * 3;
    vec2 coords = pixel_to_em(gl_FragCoord.xy) ; 
    
    vec4 color = vec4(0.);

    float min_dist = 999999.f;

    for(int i = 0; i < num_vertices; i+=3){
        ivec4 start_texel = ivec4(texelFetch(beziers, ivec2(i, 0), 0) * 255.);
        ivec4 control_texel = ivec4(texelFetch(beziers, ivec2(i + 1, 0), 0) * 255.);
        ivec4 end_texel = ivec4(texelFetch(beziers, ivec2(i + 2, 0), 0) * 255.);

        vec2 a = convert_coords(start_texel) - coords;
        vec2 b = convert_coords(control_texel) - coords;
        vec2 c = convert_coords(end_texel) - coords;

        float quad = a - 2 * b + c; 
        float linear = b - a;
        
        if(length(quad) < epsilon){
            //is linear
        }
        else{
            float d4 = dot(quad, quad);
            float d3 = dot(quad, linear);
            float d2 = dot(quad, a) + 2 * dot(linear);
            float d1 = dot(linear, a);

            //find roots of d4 t^3 + 3 d3 t^2 + d2 t + d1 = 0
            //derivative is 3 d4 t^2 + 6 d3 t + d2

            //newtons method

            float init = .5;
            float root = init;

            for(int i = 0; i < MAX_ITER; i++){
                float value = (d4 * root * root * root + 3 * d3 * root * root + d2 * root + d1)
                if(abs(value) < epsilon){
                    break;
                }
                root -= value / (3 * d4 * root * root + 6 * d3 * root + d2);
                if(root < 0){
                    root = 0;
                    break;
                }
                else if (root > 1){
                    root = 1;
                    break;
                }
            } 

        }
   }

	float coverage = sqrt(clamp(abs(winding_number), 0.0, 1.0));
    color = vec4(coverage); 

    gl_FragColor = color; 
}