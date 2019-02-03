#version 120


uniform mat4 mv_mat;

attribute vec3 position;

void main()
{
	gl_Position = mv_mat * vec4(position, 1.0);
}