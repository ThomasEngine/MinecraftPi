#shader vertex
#version 300 es
precision mediump float;

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_AtlasUV;
layout(location = 2) in vec4 a_Color;

out vec2 v_AtlasUV;
out vec4 v_Color;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(a_Position, 0.0, 1.0);
	v_AtlasUV = a_AtlasUV;
	v_Color = a_Color;
}

#shader fragment
#version 300 es
precision mediump float;

in vec2 v_AtlasUV;
in vec4 v_Color;
uniform sampler2D u_TextureAtlas;

out vec4 color;

void main()
{
	color = texture(u_TextureAtlas, v_AtlasUV) * v_Color;
}

