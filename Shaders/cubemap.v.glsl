#version 120


uniform mat4 mv_mat;

attribute vec3 position;

varying vec3 texcoord;

void main()
{
	gl_Position = mv_mat * vec4(position, 1.0);
	texcoord = position;
}