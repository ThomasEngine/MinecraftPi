#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 instancePos;
layout(location = 3) in uint atlasIndex;

out vec2 v_AtlasUV;

uniform mat4 u_MVP;
uniform float u_CellWidth;
uniform float u_CellHeight;

void main()
{
    gl_Position = u_MVP * vec4(position + instancePos, 1.0);

    // Atlas is 16x16, adjust if needed
    uint cellX = atlasIndex % 16u;
    uint cellY = atlasIndex / 16u;
	cellY += 15u;

    // OpenGL's texture origin is bottom-left, so flip Y if your atlas is top-left
    v_AtlasUV = texCoord * vec2(u_CellWidth, u_CellHeight)
              + vec2(float(cellX) * u_CellWidth, float(cellY) * u_CellHeight);
}

#shader fragment
#version 330 core

in vec2 v_AtlasUV;
layout(location = 0) out vec4 color;
uniform sampler2D u_TextureAtlas;

void main()
{
    color = texture(u_TextureAtlas, v_AtlasUV);
}
