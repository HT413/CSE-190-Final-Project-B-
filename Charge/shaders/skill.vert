// Shader for the UI
#version 330 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 TexCoords;

void main(){
	gl_Position = projection * view * model * vec4(position, 0, 1);
	TexCoords = texCoords;
}