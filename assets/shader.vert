#version 460 core

layout(location = 0) in vec2 coords;

uniform mat4 projection;
uniform mat4 model;

void main() {
	gl_Position = projection * model * vec4(coords, 0.0f, 1.0f);
}
