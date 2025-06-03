#version 330 core

in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
  // calc ambient lighting
  float ambValue = 0.1;
  vec3 ambient = objColor * lightColor * ambValue;

  // calc diffuse lighting
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm,lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // calc specular lighting
  float specularStrength = 0.5;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(reflectDir, viewDir), 0.0) ,32);
  vec3 specular = spec * specularStrength * lightColor;
  // combine
  vec3 result = (diffuse + ambient + specular) * objColor; 
  FragColor = vec4(result, 1.0); 
} 
