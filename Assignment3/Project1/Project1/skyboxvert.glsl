#version 430  // GLSL version your computer supports

in layout(location = 0) vec3 position;  

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 M;

void main()
{
	vec4 pos = projection * view * M * vec4(position, 1.0);
	gl_Position = pos;

	TexCoords = position;
}
