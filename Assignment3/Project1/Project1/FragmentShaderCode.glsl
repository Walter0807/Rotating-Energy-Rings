#version 430 //GLSL version your computer supports

out vec4 daColor;

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

uniform sampler2D texture0;
uniform sampler2D texture6;
uniform sampler2D myTextureSampler_0;
uniform sampler2D myTextureSampler_1;

uniform bool normalMapping_flag;
uniform bool sunFlag;

uniform vec3 diffuseStrength;
uniform vec3 specularStrength;

//uniform vec3 firefly0PositionWorld;

void main()
{
		vec3 normal = normalize(normalWorld);
		
		if(normalMapping_flag){
			normal = texture(myTextureSampler_1, UV).rgb;
			normal = normalize(normal*2 - 1.0);
		}

		float constant = 1f;
		float linear = 0.0035f;
		float quadratic = 0.0044f;


		//float distance = length(firefly0PositionWorld - vertexPositionWorld);
		float attenuation = 0.3;


		vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
		float brightness2 = max(dot(lightVectorWorld, normal), 0.0);
		float brightness = max(dot(lightVectorWorld, normalize(normalWorld)), 0.0);
		vec3 diffuseLight = vec3(brightness, brightness, brightness);
		vec3 diffuseLight2 = vec3(brightness2, brightness2, brightness2);
		diffuseLight = clamp(vec3(attenuation, attenuation, attenuation) * diffuseStrength * diffuseLight, 0, 1);
		diffuseLight2 = clamp(vec3(attenuation, attenuation, attenuation) * diffuseStrength * diffuseLight2, 0, 1);
		//specular light
		vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
		vec3 reflectedLightVectorWorld2 = reflect(-lightVectorWorld, normal);
		vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
		float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
		float s2 = clamp(dot(reflectedLightVectorWorld2, eyeVectorWorld), 0, 1);
		s = pow(s, 10);
		s2 = pow(s2, 10);
		vec3 specularLight = vec3(s, s, s);
				vec3 specularLight2 = vec3(s2, s2, s2);
		specularLight = vec3(attenuation, attenuation, attenuation) * specularStrength * specularLight;
		specularLight2 = vec3(attenuation, attenuation, attenuation) * specularStrength * specularLight2;
		if(!sunFlag){
			if(normalMapping_flag){
				daColor = vec4((texture(myTextureSampler_0, UV).rgb)*(ambientLight + diffuseLight2 + specularLight2), 1.0);
			}else{
				daColor = vec4((texture(texture0, UV).rgb)*(ambientLight + diffuseLight + specularLight), 1.0);
			}
		}else{
			daColor = vec4((texture(texture6, UV).rgb)*(ambientLight), 1.0);
		}
}


