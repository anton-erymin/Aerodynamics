#version 120

uniform sampler2D shadowmap;

uniform mat4 mv_mat;
uniform mat4 cam_mat;

uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform vec4 ambient_color;
uniform float shininess;


varying vec2 texcoord_f;
varying vec3 normal_f;
varying vec3 position_f;
varying vec4 shadow_position_f;
varying float fogFactor_f;



vec3 light_dir = vec3(-1.0, -2.3, -1.4);
const vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 0.0);
const vec4 light_ambient = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);



void main()
{
	vec3 eye = normalize(position_f);
	vec3 normal = normalize(normal_f);
	
	light_dir = normalize(light_dir);
	light_dir = (cam_mat * vec4(light_dir, 0.0)).xyz;

	vec4 diffamb_factor = max(-(dot(normal, light_dir)), 0.0) * light_diffuse + light_ambient;// * ambient_color;

	// Фонг
	vec3 refl = reflect(light_dir, normal);
	vec4 specular_factor = max(pow(-dot(refl, eye), shininess), 0.0) * light_specular;

	// Блинн-Фонг
	//vec3 half_vec = normalize(eye + light_dir);
	//vec4 specular_factor = max(pow(-dot(half_vec, normal), shininess), 0.0) * light_specular;
	

	float visibility = 1.0;
	
	if (shadow_position_f.x >= 0.0 && shadow_position_f.x <= 1.0 && shadow_position_f.y >= 0.0 && shadow_position_f.y <= 1.0)
	{
		if (texture2D(shadowmap, shadow_position_f.xy).x < (shadow_position_f.z - 0.01))
		{
			visibility = 0.5;
		}
	}


	vec4 finalColor = diffuse_color * diffamb_factor * visibility + specular_color * specular_factor * visibility;


	gl_FragColor = mix(vec4(0.6, 0.6, 0.65, 1.0), finalColor, fogFactor_f);
	//gl_FragColor = finalColor;
}