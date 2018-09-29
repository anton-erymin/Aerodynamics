#version 120


uniform mat4 p_mat;
uniform mat4 mv_mat;
uniform mat4 shadow_mat;

attribute vec3 position, normal;
attribute vec2 texcoord;


varying vec2 texcoord_f;
varying vec3 normal_f;
varying vec3 position_f;
varying vec4 shadow_position_f;
varying float fogFactor_f;



void main()
{
	vec4 mv_position = mv_mat * vec4(position, 1.0);
	gl_Position = p_mat * mv_position;
	position_f = mv_position.xyz;
	shadow_position_f = shadow_mat * vec4(position, 1.0);
	texcoord_f = texcoord;
	normal_f = (mv_mat * vec4(normal, 0.0)).xyz;
	
	float LOG2 = 1.442695;
	float fogDensity = 0.00005;
	float z = length(position_f);
	fogFactor_f = exp2(-fogDensity * fogDensity * z * z * LOG2 );
	fogFactor_f = clamp(fogFactor_f, 0.0, 1.0);
}