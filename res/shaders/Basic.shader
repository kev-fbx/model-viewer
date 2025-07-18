#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

out vec3 outCol;

void main()
{
    gl_Position = vec4(pos, 1.0);
    outCol = col;
};

#shader fragment
#version 330 core

out vec4 FragColor;
in vec3 outCol;

void main()
{
    FragColor = vec4(outCol, 1.0);
};