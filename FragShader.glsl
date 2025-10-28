#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D diffuse3;
uniform sampler2D specular1;
uniform sampler2D specular2;

float near = 0.1;
float far = 100.0;


void main() {
    FragColor =texture(diffuse1, TexCoords);

} 
