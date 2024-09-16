#version 460 core

in vec4 color;
in vec3 pos;
out vec4 FragColor;

void main() {
	FragColor = (color + vec4((pos + 1) / 2, 1.0f)) / 2;
}
