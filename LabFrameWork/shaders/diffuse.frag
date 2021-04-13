#version 400

in vec3 diffuseColor;

layout( location = 0 ) out vec4 FragColor;

void main() {

    FragColor = vec4(diffuseColor, 1.0);
	//FragColor *= LightIntensity;
}
