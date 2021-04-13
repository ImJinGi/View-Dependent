#version 400
layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;


out vec3 diffuseColor;

uniform vec3 LightLocation;
uniform vec3 Kd;
uniform vec3 Ld;



uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix; 
uniform mat4 MVP;

void main()
{
   vec3 N = normalize( NormalMatrix * VertexNormal);
   vec4 worldCoords = ModelViewMatrix * VertexPosition;
   vec3 L = normalize(LightLocation - worldCoords.xyz);
   diffuseColor = Kd * Ld * max(dot(L,N),0.0);
   //LightIntensity = vec3(0.5f, 0.0f, 0.0f);
   gl_Position = MVP * VertexPosition;
   
}
