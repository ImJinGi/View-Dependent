#version 450

in vec3 Position;
in vec3 Normal;

struct SpotLightInfo {
    vec4 position;   // Position in eye coords
    vec3 intensity;
    vec3 direction;  // Direction of the spotlight in eye coords.
    float exponent;  // Angular attenuation exponent
    float innerCutoff;    // Cutoff angle (between 0 and 90)
	float outerCutoff;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};
uniform SpotLightInfo Spot;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

const float constantAttenuation = 1.0;
const float linearAttenuation = 0.01f;
const float quadraticAttenuation = 0.001f;

layout( location = 0 ) out vec4 FragColor;

void main() 
{	
	//from surface to spotlight
	vec3 L = Spot.position.xyz - Position;
	vec3 N = normalize(Normal);
	float dist_surface_lp = length(L);
	L = normalize(L);

    float attenuation = 1.0f /  (constantAttenuation +
                          linearAttenuation * dist_surface_lp +
                          quadraticAttenuation * (dist_surface_lp * dist_surface_lp));

	float spotdot = dot(-L, normalize(Spot.direction));
	//current surface's fragment angle
	float angle = acos(spotdot);
	float spotAttenuation;

	if(angle < (radians(Spot.innerCutoff)))
	{
		spotAttenuation = 1.0f;
	}
	else
	{
		float spotValue=smoothstep(cos(radians(Spot.outerCutoff)), cos(radians(Spot.innerCutoff)), spotdot);
		spotAttenuation = pow(spotValue, Spot.exponent);
	}
	attenuation *= spotAttenuation;

	vec3 V = normalize(vec3(-Position));
	vec3 H = normalize(V + L);

	float n_dot_L = max (0.f, dot(Normal, L));
	float n_dot_H = max (0.f, dot(Normal, H));
	float powerFactor = pow(n_dot_H, Shininess);

	vec3 ambient  = Spot.La * Ka * Spot.intensity * attenuation;
	vec3 diffuse  = Spot.Ld * Kd * Spot.intensity  * n_dot_L * attenuation;
	vec3 specular = Spot.Ls * Ks * Spot.intensity * attenuation * powerFactor;

	FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
