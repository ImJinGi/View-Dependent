#version 440

in float Transp;
in vec2 TexCoord;
in vec3 ParticlePos;

uniform sampler2DArray particleTexArray;
uniform int layer;

struct SpotLightInfo
{
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

const float PI = 3.141592653589793;
const float constantAttenuation = 1.0;
const float linearAttenuation = 0.01f;
const float quadraticAttenuation = 0.001f;

layout ( location = 0 ) out vec4 FragColor;

vec3 insideCone(vec3 pos, vec3 lightPos, vec3 lightDir, vec3 lightCol, float aperture);
void insideCone(vec3 pos, vec3 lightPos, vec3 lightDir, vec3 lightCol, float aperture, out vec3 particlePos, out vec3 textureColor);
vec3 detectCone(vec3 pos, vec3 lightPos, vec3 lightDir, vec3 lightColor);

void main()
{
	vec3 position = ParticlePos;
	vec3 lightPos = Spot.position.xyz;
	vec3 lightDir = normalize(-Spot.direction);
	vec3 lightCol = Spot.La + Spot.Ld + Spot.Ls;

	float aperture = radians(30.0f);
	
	//vec3 rgb = insideCone(position, lightPos, lightDir, lightCol, aperture);
	vec3 rgb = detectCone(position, lightPos, lightDir, lightCol);
	
	FragColor = vec4(rgb, texture(particleTexArray, vec3(TexCoord, layer)));
	//FragColor = texture(particleTexArray, vec3(TexCoord, layer));
}


vec3 detectCone(vec3 particle_pos, vec3 lightPos, vec3 lightDir, vec3 lightColor)
{
	vec3 tip = lightPos;
	vec3 dir = normalize(lightDir);
	float height = lightPos.y;
	float radius;

	vec3 point = particle_pos;

	vec3 point_to_tip = particle_pos - tip;
	float dot_projPoint_dir = dot(point_to_tip, dir);	

	// when particles are above the light
	if(dot_projPoint_dir <= 0 || dot_projPoint_dir >= height)
	{
		return vec3(0.0f);
	}

	float theta = dot_projPoint_dir / (length(point_to_tip) * length(dir));
	float cos_theta = cos(theta);
	float hypotenuse = 0.0f;
	float sin_theta;
	if(cos_theta != 0)
	{
		hypotenuse = height / cos_theta;
		sin_theta = sin(theta);
		if(sin_theta != 0)
		{
			radius = hypotenuse * sin_theta;
		}
	}
	
	float orthDistance = length((point_to_tip) - dot_projPoint_dir * dir);
	
	// when particles are affected by light
	if(orthDistance < radius)
	{
		vec3 L = lightPos - particle_pos;
		float dist_surface_lp = length(L);
		L = normalize(L);
		float attenuation = 1.0f /  (constantAttenuation + linearAttenuation * dist_surface_lp + quadraticAttenuation * (dist_surface_lp * dist_surface_lp));
		float spotdot = dot(-L, normalize(lightDir));
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

		return texture(particleTexArray, vec3(TexCoord, layer)).rgb * lightColor * Spot.intensity * attenuation;
	}
	// when particles are not affected by light
	else
	{
		return vec3(0, 0, 0);
	}
}


vec3 insideCone(vec3 pos, vec3 lightPos, vec3 lightDir, vec3 lightCol, float aperture)
{
	vec3 tip = lightPos;
	vec3 dir = normalize(lightDir);
	float tanTheta = tan(aperture);
	float height = lightPos.y;
	float radius = height * tanTheta;

	float cone_dist = dot(pos - tip, dir);
	
	// when particles are not affected by light
	if(cone_dist <= 0 || cone_dist >= height)
	{
		return vec3(1, 0, 0);
	}

	float cone_radius = (cone_dist / height) * radius;

	float orth_distance = length((pos - height) - cone_dist * dir);
	
	if(orth_distance < cone_radius)
	{
		return vec3(0, 1, 0);
	}
	else
	{
		return vec3(0, 0, 1);
	}
}
