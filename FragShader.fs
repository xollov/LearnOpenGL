#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float     shininess;
};
struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
  // calc ambient lighting
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  // calc diffuse lighting
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm,lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

  // calc specular lighting
  float specularStrength = 0.5;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(reflectDir, viewDir), 0.0) ,32);
  vec3 specularMap = vec3(texture(material.specular, TexCoords));
  vec3 specular = light.specular * spec * specularMap;
  // combine
  vec3 result = diffuse + ambient + specular; 
  // add emmision
  if (specularMap == vec3(0)) {
    result += vec3(texture(material.emission, TexCoords)); 
  }
  vec4 tex = texture(material.diffuse, TexCoords);
  FragColor = vec4(result,1.0); 
} 
