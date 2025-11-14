#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in uint blockType;
layout(location = 3) in uint face;
layout(location = 4) in uint facesPerBlock;

out vec2 v_TexCoord;
flat out uint v_FacesPerBlock;
flat out uint v_BlockType;
flat out uint v_Face;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
    v_BlockType = blockType;
    v_Face = face;
	v_FacesPerBlock = facesPerBlock;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
flat in uint v_BlockType;
flat in uint v_Face;
flat in uint v_FacesPerBlock;

uniform sampler2D u_TextureAtlas;
uniform int u_AtlasCols;
uniform int u_AtlasRows;

void main()
{
    int facesPerBlock = int(v_FacesPerBlock);
	
    int faceIndex = (facesPerBlock == 1) ? 0 : int(v_Face);
    int atlasIndex = int(v_BlockType) * facesPerBlock + faceIndex;
    int cols = u_AtlasCols;
    int rows = u_AtlasRows;
    float cellWidth = 1.0 / float(cols);
    float cellHeight = 1.0 / float(rows);

    int cellX = atlasIndex % cols;
    int cellY = atlasIndex / cols;

    vec2 atlasUV = v_TexCoord * vec2(cellWidth, cellHeight) + vec2(cellX * cellWidth, cellY * cellHeight);

    color = texture(u_TextureAtlas, atlasUV);
}

