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
    // FragColor = mix(texture(diffuse1,TexCoords), texture(specular1, TexCoords), 0.2);

    float z = gl_FragCoord.z * 2.0 - 1;
    float depth = (2.0 * near * far) / (far + near - z * (far - near));
    depth /= far;

    FragColor = vec4(vec3(depth), 1.0);
} 
