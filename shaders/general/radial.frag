#version 330 core

in vec4 gl_FragCoord;

out vec4 FragColor;

uniform float lineThickness = 0.5f; // thickness of line 0.5f = per pixel
uniform vec2 position = vec2(-256,-256);
uniform vec2 scale = vec2(1, 1);

vec2 new_FragCoord = vec2((gl_FragCoord.x+position.x)*scale.x, (gl_FragCoord.y+position.y)*scale.y);
vec2 new_lineThickness = vec2(lineThickness*scale.x, lineThickness*scale.y); 

float f(float x); // math function being vizualized float x, y = pixel positions

float e = 2.71828;
float z = 1.0f;
float a = 1.0f;
float pi = 3.14159;

float theta = 0.0f;
float phi = 0.0f;

void main()
{
    float xmin = new_FragCoord.x-new_lineThickness.x;
    float xmax = new_FragCoord.x+new_lineThickness.x;
    float ymin = new_FragCoord.y-new_lineThickness.y;
    float ymax = new_FragCoord.y+new_lineThickness.y;

    for(float i = xmin; i <= xmax; i += 0.01f*scale.x)
    {
        float y = pow(i, 2.0f)*pow(f(i), 2.0f);
        if (y >= ymin && y <= ymax)
        {
            FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        }else if
        (
            new_FragCoord.x >= -new_lineThickness.x/2 && 
            new_FragCoord.x <= new_lineThickness.x/2 || 
            new_FragCoord.y >= -new_lineThickness.y/2 && 
            new_FragCoord.y <= new_lineThickness.y/2)
        {
            FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        } else 
        {
            FragColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
        }
    }
}