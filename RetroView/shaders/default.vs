#version 330

layout(location = 0) in vec3 in_Vertex;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Vertex, 1.0f);
}

