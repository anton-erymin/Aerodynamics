#version 110

uniform sampler2D texture;


varying vec2 f_texcoord;
varying vec4 diffamb_factor;
varying vec4 specular_factor;


void main()
{
	//gl_FragColor = texture2D(texture, f_texcoord) * diffamb_factor + vec4(0.6f, 0.6f, 0.0f, 1.0f) * specular_factor;
	gl_FragColor = vec4(1.0f, 0.4f, 0.7f, 1.0f) * diffamb_factor + vec4(0.6f, 0.6f, 0.0f, 1.0f) * specular_factor;
}