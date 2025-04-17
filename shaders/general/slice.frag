#version 330 core

in vec4 gl_FragCoord;

out vec4 FragColor;

uniform float lineThickness = 0.5f; // thickness of line 0.5f = per pixel
uniform vec2 position = vec2(-256,-256);
uniform vec2 scale = vec2(1, 1);
uniform float intensity = 1.0f;
uniform int plane = 1;

vec3 new_FragCoord = vec3((gl_FragCoord.x+position.x)*scale.x, (gl_FragCoord.y+position.y)*scale.y, 0.0f);
vec2 new_lineThickness = vec2(lineThickness*scale.x, lineThickness*scale.y); 

float f(float x); // math function being vizualized float x, y = pixel positions
float g(float x); // math function being vizualized float x, y = pixel positions

float e = 2.71828;
float z = 1.0f;
float a = 1.0f;
float pi = 3.14159;

float r = pow(pow(new_FragCoord.x, 2.0f) + pow(new_FragCoord.y,2.0f),0.5f);
float theta;
float phi;

void main()
{
    if (plane == 1)
    {
        theta = abs(acos(new_FragCoord.y/r));
        phi = 0.0f;
    } else if (plane == 2)
    {
        theta = pi/2.0f;
        phi = abs(acos(new_FragCoord.x/r));
    } else if (plane == 3)
    {
        theta = abs(acos(new_FragCoord.y/r));
        phi = pi/2.0f;
    }
    float h = abs(f(r)) * abs(g(r));

    vec3 temp_FragColor = vec3(1.0f, 1.0f, 1.0f) * h * intensity;
    FragColor = vec4(temp_FragColor.x, temp_FragColor.y, temp_FragColor.z, 1.0f);
}