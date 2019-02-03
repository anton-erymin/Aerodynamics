#version 110


uniform mat4 p_mat;
uniform mat4 mv_mat;

attribute vec3 position, normal;
attribute vec2 texcoord;


varying vec2 f_texcoord;


// Освещенность
varying vec4 diffamb_factor;
varying vec4 specular_factor;


const vec3 light_dir = vec3(1.0f, 0.0f, 0.0f);
const vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 0.0);
const vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1.0);
const vec4 light_specular = vec4(1.0, 1.0, 0.0f, 1.0);

void main()
{
	vec4 mv_position = mv_mat * vec4(position, 1.0f);
	gl_Position = p_mat * mv_position;

	f_texcoord = texcoord;

	vec3 mv_normal = (mv_mat * vec4(normal, 0.0f)).xyz;
	mv_normal = normalize(mv_normal);

	vec3 eye = mv_position.xyz;
	eye = normalize(eye);


	vec3 refl = reflect(light_dir, mv_normal);

	diffamb_factor = max(dot(mv_normal, light_dir), 0.0) * light_diffuse + light_ambient;

	specular_factor = max(pow(dot(refl, eye), 50), 0.0) * light_specular;


}