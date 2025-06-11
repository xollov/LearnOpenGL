#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;
uniform vec3 lightColor;
void main()
{
  FragColor = vec4(lightColor, 1.0); 
} 
