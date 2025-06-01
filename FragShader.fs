#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 objColor;

void main()
{
  float ambValue = 0.4f;
  vec3 amb = objColor * lightColor * ambValue;
  FragColor = vec4(amb, 1); 
} 
