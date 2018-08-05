#version 330 core
in  vec2  TexCoords;
out vec4  FragColor;

uniform sampler2D scene;
uniform sampler2D texCut;

void main()
{
	if(texture(texCut, TexCoords).r < 0.5)
		FragColor =  texture(scene, TexCoords);
	else
		FragColor =  vec4(0.0, 0.0, 0.0, 1.0);
}