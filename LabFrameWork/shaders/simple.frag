#version 400

in vec3 f_color;

out vec4 FragColors;

void main()
{
   FragColors = vec4(f_color, 1.0);
}