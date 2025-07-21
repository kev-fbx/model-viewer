#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 inTexCoord;

out vec3 outCol;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(pos, 1.0);
    outCol = col;
    texCoord = inTexCoord;
};

#shader fragment
#version 330 core

out vec4 FragColor;
in vec3 outCol;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, texCoord) * vec4(outCol, 1.0);
};