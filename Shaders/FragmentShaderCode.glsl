#version 430 //GLSL version your computer supports

out vec4 daColor;

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 ambientLight;
uniform vec3 sunAmbient;
uniform vec3 lightPositionWorld;
uniform vec3 lightPositionWorld2;
uniform vec3 eyePositionWorld;

uniform sampler2D texture0;
uniform sampler2D texture6;
uniform sampler2D myTextureSampler_0;
uniform sampler2D myTextureSampler_1;

uniform bool normalMapping_flag;
uniform bool sunFlag;
uniform bool redSunFlag;

uniform vec3 diffuseStrength;
uniform vec3 diffuseStrength2;
uniform vec3 specularStrength;
uniform vec3 specularStrength2;


void main()
{
		vec3 normal = normalize(normalWorld);
		
		if(normalMapping_flag){
			normal = texture(myTextureSampler_1, UV).rgb;
			normal = normalize(normal*2 - 1.0);
		}

		float constant = 1.0f;
		float linear = 0.00035f;
		float quadratic = 0.00044f;


		float dis = length(lightPositionWorld - vertexPositionWorld);
		dis = dis/40;
		float dis2 = length(lightPositionWorld2 - vertexPositionWorld);
		dis2 = dis2/40;
		float attenuation = 1.0f/(1.0f+linear*dis+quadratic*pow(dis,2));
		float attenuation2 = 1.0f/(1.0f+linear*dis2+quadratic*pow(dis2,2));


		vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
		vec3 lightVectorWorld2 = normalize(lightPositionWorld2 - vertexPositionWorld);
		float brightness2 = max(dot(lightVectorWorld2, normalize(normalWorld)), 0.0);
		float brightness = max(dot(lightVectorWorld, normalize(normalWorld)), 0.0);
		vec3 diffuseLight = vec3(brightness, brightness, brightness);
		vec3 diffuseLight2 = vec3(brightness2, brightness2, brightness2);
		diffuseLight = clamp(vec3(attenuation, attenuation, attenuation) * diffuseStrength * diffuseLight, 0, 1);
		diffuseLight2 = clamp(vec3(attenuation2, attenuation2, attenuation2) * diffuseStrength2 * diffuseLight2, 0, 1);
		//specular light
		vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normal);
		vec3 reflectedLightVectorWorld2 = reflect(-lightVectorWorld, normal);
		vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
		float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
		float s2 = clamp(dot(reflectedLightVectorWorld2, eyeVectorWorld), 0, 1);
		s = pow(s, 100);
		s2 = pow(s2, 100);
		vec3 specularLight = vec3(s, s, s);
		vec3 specularLight2 = vec3(s2, s2, s2);
		specularLight = vec3(attenuation, attenuation, attenuation) * specularStrength * specularLight;
		specularLight2 = vec3(attenuation2, attenuation2, attenuation2) * specularStrength * specularLight2;
		if(!sunFlag){
			if(normalMapping_flag){
				vec3 temp00 = texture(myTextureSampler_0, UV).rgb;

				if(redSunFlag){
					daColor = vec4(vec3(temp00.x*0.8, temp00.y*0.45, temp00.z*0.56)*(ambientLight + diffuseLight2 + specularLight2), 1.0) 
					+ 0.3*vec4((temp00)*(ambientLight + diffuseLight + specularLight), 1.0);
				}else{
					daColor = vec4((temp00)*(ambientLight + diffuseLight + specularLight), 1.0);
				}
				
			}else{
				vec3 temp00 = texture(texture0, UV).rgb;
				if(redSunFlag){
					daColor = vec4(vec3(temp00.x*0.8, temp00.y*0.45, temp00.z*0.56)*(ambientLight + diffuseLight2 + specularLight2), 1.0) + 0.3*vec4((temp00)*(ambientLight + diffuseLight + specularLight), 1.0);
				}else{
					daColor = vec4((temp00)*(ambientLight + diffuseLight + specularLight), 1.0);
				}
			
			}
		}else{
			if(redSunFlag){
				daColor = vec4((texture(texture6, UV).rgb)*(ambientLight), 1.0) + 0.3*vec4((texture(texture6, UV).rgb)*(ambientLight), 1.0);			
			}else{
				daColor = vec4((texture(texture6, UV).rgb)*(ambientLight), 1.0);			
			}

		}
}


