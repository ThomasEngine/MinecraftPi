#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float cellX;
layout(location = 3) in float cellY;

out vec2 v_AtlasUV;

uniform mat4 u_MVP;
uniform float u_CellWidth;
uniform float u_CellHeight;

void main()
{
    gl_Position = u_MVP * vec4(position, 1.0);
    v_AtlasUV = texCoord * vec2(u_CellWidth, u_CellHeight) + vec2(cellX * u_CellWidth, cellY * u_CellHeight);
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_AtlasUV;
uniform sampler2D u_TextureAtlas;

void main()
{
    color = texture(u_TextureAtlas, v_AtlasUV);
}


