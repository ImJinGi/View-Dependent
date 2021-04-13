#version 440

const float PI = 3.14159265359;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexAge;
layout (location = 3) in vec4 VertexInitPos;
layout (location = 4) in vec3 VertexLayer;

layout( xfb_buffer = 0, xfb_offset = 0) out vec3 Position;
layout( xfb_buffer = 1, xfb_offset = 0) out vec3 Velocity;
layout( xfb_buffer = 2, xfb_offset = 0) out float Age;

out float Transp;           // Transparency
out vec2 TexCoord;
out vec3 particle_layer;

//uniform float Time;
uniform float DeltaT;                    
uniform float ParticleLifetime;                              
//uniform mat4 MV;    // View * Model
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;  // Projection matrix

uniform float ParticleSize;       // Size of particle

// Offsets to the position in camera coordinates for each vertex of the particle's quad
//const vec3 offsets[] = vec3[](vec3(-0.5,-0.5,0), vec3(0.5,-0.5,0), vec3(0.5,0.5,0),
//                              vec3(-0.5,-0.5,0), vec3(0.5,0.5,0), vec3(-0.5,0.5,0) );
const vec4 offsets[] = vec4[](vec4(-0.5, -3.3, 0, 0), vec4(0.5, -3.3, 0, 0), vec4( 0.5, 3.3,0, 0),
                              vec4(-0.5, -3.3, 0, 0), vec4(0.5,  3.3, 0, 0), vec4(-0.5, 3.3,0, 0) );
// Texture coordinates for each vertex of the particle's quad
const vec2 texCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,0), vec2(1,1), vec2(0,1));

subroutine (RenderPassType)
void update() 
{

	if( VertexAge < 0 || VertexAge > ParticleLifetime ) 
	{
        // The particle is past it's lifetime, recycle.
		Position = VertexInitPos.xyz;
        Velocity = vec3(0, -5.0f, 0);//randomInitialVelocity();
        if( VertexAge < 0 ) 
		{
			Age = VertexAge + DeltaT;
		}
		else 
		{
			Age = (VertexAge - ParticleLifetime) + DeltaT;
		}

    } 
	else 
	{
        // The particle is alive, update.
        Position = VertexPosition.xyz + VertexVelocity.xyz * DeltaT;
        Velocity = VertexVelocity.xyz;// + Accel * DeltaT;
        Age = VertexAge + DeltaT;
    }
    
	//particle's layer
	particle_layer = VertexLayer;
}

subroutine (RenderPassType)
void render() 
{
	Transp = 0.0;
	vec4 posCam = vec4(0.0);
	if (VertexAge >= 0.0) 
	{
		//posCam = (vec4(VertexPosition, 1) * View).xyz + offsets[gl_VertexID] * ParticleSize;
		posCam = (vec4(VertexPosition.xyz, 1)) + offsets[gl_VertexID] * ParticleSize;
		Transp = clamp(1.0 - VertexAge / ParticleLifetime, 0, 1);
	}
	TexCoord = texCoords[gl_VertexID];
	gl_Position = Proj * View * Model * posCam;//vec4(posCam, 1);
}

void main() {

	RenderPass();
}

