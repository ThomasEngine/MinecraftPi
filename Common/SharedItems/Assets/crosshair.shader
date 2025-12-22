#shader vertex
#version 300 es
precision mediump float;
layout(location = 0) in vec3 aPos;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
}

#shader fragment
#version 300 es
precision mediump float;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 0.6);
}