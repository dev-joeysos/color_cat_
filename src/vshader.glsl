#version 430

in  vec4 vPosition;
in  vec4 vNormal;
in  vec2 vTexCoord;

out vec4 fragPos;
out vec4 color;
out vec4 normal;
out vec2 texCoord;

uniform mat4 mModel;
uniform mat4 mPVM;
uniform int isTexture;

void main() 
{
	gl_Position = mPVM * vPosition;

	vec4 vColor = vec4(0, 1, 0, 1);
	if (isTexture == 1) {
		vColor = vec4(1, 1, 1, 1);
	}
	vec4 L = normalize(vec4(3, 3, 5, 0));

	float kd = 0.8, ks = 1.0, ka = 0.2, shininess = 60;
	vec4 Id = vColor;
	vec4 Is = vec4(1, 1, 1, 1);
	vec4 Ia = vColor;

	fragPos = mModel * vPosition;
	normal = transpose(inverse(mModel)) * vNormal;
	color = vColor;

	// texture coordinate
	texCoord = vTexCoord;
}