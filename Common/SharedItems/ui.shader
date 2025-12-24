#shader vertex
#version 300 es
precision mediump float;

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

uniform mat4 u_MVP;

out vec2 vUV;
out vec4 vColor;

void main() {
    vUV = aUV;
    vColor = aColor;
    gl_Position = u_MVP * vec4(aPos, 0.0, 1.0);
}

#shader fragment
#version 300 es
precision mediump float;

in vec2 vUV;
in vec4 vColor;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main() {
    FragColor = texture(u_Texture, vUV) * vColor;
    //FragColor = vec4(1.0);
}



