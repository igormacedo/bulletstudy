#version 430 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

//uniform mat4 modelTransformMatrix;
//uniform mat4 projectionMatrix;
uniform mat4 mvpMatrix;

out vec3 color;

void main()
{
	//gl_Position = vec4(in_position, 1);
	vec4 v = vec4(in_position, 1);
	//vec4 newPosition = modelTransformMatrix * v;
	//vec4 projectedPosition = projectionMatrix * newPosition;
	vec4 projectedPosition = mvpMatrix * v;
	gl_Position = projectedPosition;
	//gl_Position = newPosition;
	color = in_color;
}