#version 430 //GLSL version your computer supports

in vec3 TexCoords;

out vec4 color;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox, TexCoords);
}


