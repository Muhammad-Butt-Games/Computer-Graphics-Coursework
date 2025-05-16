#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D diffuseMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform float ka; // Ambient coefficient
uniform float kd; // Diffuse coefficient
uniform float ks; // Specular coefficient
uniform float Ns; // Shininess

void main()
{
    // Ambient
    vec3 ambient = ka * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = ks * spec * lightColor;

    // Combine
    vec3 lighting = ambient + diffuse + specular;

    vec4 texColor = texture(diffuseMap, TexCoord);
    FragColor = vec4(lighting, 1.0) * texColor;
}
