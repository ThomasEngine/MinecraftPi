#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_CellX;
layout(location = 3) in float a_CellY;
layout(location = 4) in float a_Light;

out vec2 v_AtlasUV;
out float v_LightLevel;

uniform mat4 u_MVP;
uniform float u_CellWidth;
uniform float u_CellHeight;
uniform float u_DayTime;

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    v_AtlasUV = a_TexCoord * vec2(u_CellWidth, u_CellHeight) + vec2(a_CellX * u_CellWidth, a_CellY * u_CellHeight);
    v_LightLevel = a_Light * u_DayTime;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in float v_LightLevel;
in vec2 v_AtlasUV;
uniform sampler2D u_TextureAtlas;

void main()
{
    color = texture(u_TextureAtlas, v_AtlasUV);
    color.rgb *= v_LightLevel;
}


