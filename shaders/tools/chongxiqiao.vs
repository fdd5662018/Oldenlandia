#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 tangentFragPos;
out vec3 tangentViewPos;
out vec3 tangentLightPos[1];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPositions[1];
uniform vec3 camPos;

void main()
{
    TexCoords = aTexCoords;
    vec3 WorldPos = vec3(model * vec4(aPos, 1.0));
	//TBN
    mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));
	//world to TBN
	tangentFragPos = TBN * WorldPos;
	tangentViewPos  = TBN * camPos;
	for(int i = 0; i < 1; i++){
		tangentLightPos[i]  = TBN * lightPositions[i];
	}

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}