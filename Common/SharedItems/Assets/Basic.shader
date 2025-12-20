#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_CellX;
layout(location = 3) in float a_CellY;
layout(location = 4) in float a_Light;
layout(location = 5) in float a_AO;

out vec2 v_AtlasUV;
out float v_LightLevel;
out float v_DayTime;
out float v_FogFactor;
out float v_UnderWater;

uniform mat4 u_MVP;
uniform float u_CellWidth;
uniform float u_CellHeight;
uniform float u_DayTime;
uniform float u_UnderWater;
uniform float u_YOffset;

float near = 90.0f;                    
float far = 110.0f;      

void main()
{
    vec3 Position = a_Position;
    Position.y += u_YOffset;
    gl_Position = u_MVP * vec4(Position, 1.0);
    v_AtlasUV = a_TexCoord * vec2(u_CellWidth, u_CellHeight) + vec2(a_CellX * u_CellWidth, a_CellY * u_CellHeight);
    v_LightLevel = a_Light * u_DayTime * a_AO;
    v_DayTime = u_DayTime;
    v_UnderWater = u_UnderWater;
    float dist = length(gl_Position.xyz);
    v_FogFactor = clamp((far - dist) / (far - near), 0.0, 1.0);
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in float v_LightLevel;
in vec2 v_AtlasUV;
in float v_DayTime;
in float v_FogFactor;
in float v_UnderWater;   
uniform sampler2D u_TextureAtlas;


vec3 fogColor = vec3(0.53 * v_DayTime, 0.81 * v_DayTime, 0.92 * v_DayTime);
vec3 underWaterColor = vec3(0.0, 0.4, 0.7);

void main()
{
    color = texture(u_TextureAtlas, v_AtlasUV);
    float alpha = color.a;
    color.rgb *= v_LightLevel;
    color.rgb = mix(underWaterColor, color.rgb,  1 - v_UnderWater * 0.25);
    color.rgb = mix(fogColor, color.rgb, v_FogFactor);
    color.a = alpha;
}



