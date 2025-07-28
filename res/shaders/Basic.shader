#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    texCoord = inTexCoord;
};

#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, texCoord);
}