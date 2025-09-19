#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 inTexCoord;

out vec2 texCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 actualNormal = normal;
    if (length(normal) < 0.1) {
        actualNormal = vec3(0.0, 1.0, 0.0);
    }
    
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * actualNormal;
    texCoord = inTexCoord;
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture1;

uniform vec3 lightPos = vec3(1.2, 1.0, 2.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 viewPos = vec3(0.0, 0.0, 3.0);

void main()
{
    vec4 texColor;
    if (textureSize(texture_diffuse1, 0).x > 1) {
        texColor = texture(texture_diffuse1, texCoord);
    } else {
        texColor = texture(texture1, texCoord);
    }
    
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (ambient + diffuse) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}