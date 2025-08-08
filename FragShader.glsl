#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D diffuse3;
uniform sampler2D specular1;
uniform sampler2D specular2;

void main() {
    FragColor = mix(texture(diffuse1,TexCoords), texture(specular1, TexCoords), 0.2);
} 
