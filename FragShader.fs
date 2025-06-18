#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float     shininess;
};
struct Light {
  vec3 position;
  vec3 spotDirection;
  float cutOff;
  float outerCutOff;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  
  float constant;
  float linear;
  float quadratic;
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
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);

  float distance = length(light.position - FragPos);
  float attenuation = 1 / (light.constant + distance * light.linear + distance * distance * light.quadratic);

  float theta = dot(lightDir, -light.spotDirection);
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  vec3 result;
  if (light.outerCutOff < theta) {
    // calc ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // calc diffuse lighting
    float diff = max(dot(norm,lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // calc specular lighting
    vec3 viewdir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewdir), 0.0) ,32);
    vec3 specularMap = vec3(texture(material.specular, TexCoords));
    vec3 specular = light.specular * spec * specularMap;

    // combine
    diffuse *= intensity;
    specular *= intensity;
    result = (diffuse + ambient + specular) * attenuation ; 
    // add emmision
    //if (specularmap == vec3(0)) {
    //  result += vec3(texture(material.emission, texcoords)); 
    //}
  } else {
    result = attenuation * light.ambient * vec3(texture(material.diffuse, TexCoords));
    //result = vec3(0.6, 0.2, 0.4);
  }
  vec4 tex = texture(material.diffuse, TexCoords);
  FragColor = vec4(result,1.0); 
} 
