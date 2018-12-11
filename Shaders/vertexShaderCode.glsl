#version 430  // GLSL version your computer supports

in layout(location = 0) vec3 position;  //vbo
in layout(location = 1) vec2 vertexUV;  //uvbo -- uv is being used to calculate texture, which is equivalent to color calculation
in layout(location = 2) vec3 vertexNormal; //nvbo

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 MVP;

out vec2 UV;
out vec3 normalWorld;
out vec3 vertexPositionWorld;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 newPosition = model * v;
	vec4 projectedPosition = projection * newPosition;

	//calculate world coordinate of vertex and normal
	vec4 normal_temp = model * vec4(vertexNormal, 0);
	normalWorld = normal_temp.xyz;
	vertexPositionWorld = newPosition.xyz;

	gl_Position = MVP*v;
	UV = vertexUV;

}
