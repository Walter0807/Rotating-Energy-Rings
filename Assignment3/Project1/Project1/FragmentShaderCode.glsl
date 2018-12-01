#version 430 //GLSL version your computer supports

out vec4 daColor;

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

uniform sampler2D texture0;
uniform sampler2D myTextureSampler_0;
uniform sampler2D myTextureSampler_1;

uniform bool normalMapping_flag;

uniform vec3 diffuseStrength;
uniform vec3 specularStrength;

//uniform vec3 firefly0PositionWorld;



void main()
{
	vec3 normal = normalize(normalWorld);
	//if(normalMapping_flag){
		normal = texture(myTextureSampler_1, UV).rgb;
		normal = normalize(normal*2 - 1.0);
	//}



	float constant = 1f;
	float linear = 0.0035f;
	float quadratic = 0.0044f;


	//float distance = length(firefly0PositionWorld - vertexPositionWorld);
	float attenuation = 0.3;


	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	//float brightness = max(dot(lightVectorWorld, normal), 0.0);
	float brightness = max(dot(lightVectorWorld, normalize(normalWorld)), 0.0);
	vec3 diffuseLight = vec3(brightness, brightness, brightness);
	diffuseLight = clamp(vec3(attenuation, attenuation, attenuation) * diffuseStrength * diffuseLight, 0, 1);

	//specular light
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s, 10);
	vec3 specularLight = vec3(s, s, s);
	specularLight = vec3(attenuation, attenuation, attenuation) * specularStrength*specularLight;

	//ambientLight  = 0 * ambientLight;
	//ambientLight  = vec3(attenuation , attenuation, attenuation) * ambientLight;
	//diffuseLight = vec3(attenuation, attenuation, attenuation) *attenuation;
	//specularLight = vec3(attenuation, attenuation, attenuation) *attenuation;

	daColor = vec4((texture(texture0, UV).rgb + normal)*(ambientLight + diffuseLight + specularLight), 1.0);

}


