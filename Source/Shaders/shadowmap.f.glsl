#version 120


void main()
{
	//gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	//gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
	//gl_FragDepth = gl_FragCoord.z;
	gl_FragColor.r = gl_FragCoord.z;
}