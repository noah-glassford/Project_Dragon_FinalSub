#version 420
//Credit: http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl / https://www.shadertoy.com/view/MdlGWX
//modified to work within this framework, modified some of the values and added the framebuffer texture

in vec2 texcoord; // Texture coordinates from the vertex shader

layout (binding = 0) uniform sampler2D s_screenTex;

layout(location = 0) out vec4 out_color;



uniform float time; // Time since the start of the frame
uniform vec2 iResolution;
const float noiseStrength = 30.0; // The strength of the noise effect
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

uniform float u_Intensity;

void main()
{
   
	//Fragment position, only used as random seed
	vec2 uv = texcoord.xy; 
	
	//Flicker frequency
	float flicker = 1.0;
		
	//Play with power to change noise frequency
	float freq = sin(pow(mod(time, flicker)+flicker, 1.9));
	
	//Play with this to change raster dot size (x axis only, y is calculated with aspect ratio)
	float pieces = float(200);
	
	//Calculations to maintain square pixels
	float ratio_x = 1.0 / pieces;
	float ratio_y = ratio_x * iResolution.x / iResolution.y;
	float half_way_x = abs(freq * ratio_x);
	float half_way_y = abs(freq * ratio_y);
	
	//Checkerboard generation
	float x_pos = mod(uv.x, ratio_x);
	float y_pos = mod(uv.y, ratio_y);
	if(x_pos < half_way_x && y_pos < half_way_y)
		out_color = vec4(0.5,0.5,0.5,0.5);
	else if(x_pos < half_way_x && y_pos > half_way_y)
		out_color -= vec4(0.0,0.0,0.0,0.0);
	else if(x_pos > half_way_x && y_pos < half_way_y)
		out_color -= vec4(0.0,0.0,0.0,0.0);
	else
		out_color = vec4(0.5,0.5,0.5,0.5);
		
	//Comment this out to see how raster dots are simulated (noise overlay)
	//fragColor *= vec4(rand(uv+mod(time, freq)), rand(uv+mod(time+.1, freq)), rand(uv), 1.0);
	
	//Use this for greyscale noise, comment out the line above (noise overlay)
	out_color *= vec4(rand(uv+mod(time, freq)), rand(uv+mod(time, freq)), rand(uv+mod(time, freq)), 0.05) * u_Intensity;
	out_color += texture(s_screenTex, texcoord);
	
}