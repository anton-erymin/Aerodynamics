#version 120

uniform samplerCube texture;

varying vec3 texcoord;

void main()
{
	gl_FragColor = textureCube(texture, texcoord);
}