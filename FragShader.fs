#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float     shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINTS 4
uniform PointLight  pointLights[NR_POINTS];

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight;


vec3 calcDirectional(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPoint(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirectional(directionalLight, normal,viewDir);
    
    for (int i = 0; i < NR_POINTS; i++) {
        result += calcPoint(pointLights[i], normal, FragPos, viewDir);
    }
    FragColor = vec4(result, 1);
} 
vec3 calcDirectional(DirectionalLight light, vec3 normal, vec3 viewDir) {

    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(-lightDir, normal), 0.0);
    
    vec3 reflectDirection = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDirection, viewDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); 
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); 

    return ambient + diffuse + specular;
}
vec3 calcPoint(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(lightDir, normal), 0.0);
   
    vec3 reflectDirection = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDirection, viewDir), 0.0), material.shininess);

    float distance = length(lightDir - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); 
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); 
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); 

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

