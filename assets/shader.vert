#version 460 core

layout(location = 0) in vec2 coords;
layout(location = 1) in mat4 model;
layout(location = 5) in vec4 i_color;

out vec4 color;
out vec3 pos;

uniform mat4 projection;

void main() {
	color = i_color;
	gl_Position = projection * model * vec4(coords, 0.0f, 1.0f);
	pos = gl_Position.xyz;
}
