#version 460 core

out vec4 FragColor;

in vec3 pos;
in vec3 normal;
in vec2 texCoord;
in float face;


uniform sampler2D tex0;

uniform vec3 camPos;
uniform vec3 camDir;
uniform float time;

float CalcExpFogFactor(vec3 worldpos)
{
	float fogend = 40 * 32;

    float CameraToPixelDist = length(worldpos - camPos);
    float DistRatio = 4.0 * CameraToPixelDist / fogend;
    float FogFactor = 0.0;
	float fogdensity = 0.2f;

    FogFactor = max(exp(-DistRatio * fogdensity * DistRatio * fogdensity), 0.5);
    return FogFactor;
}

void main()
{
	const float ambient = 0.5f;


	float fogfactor = CalcExpFogFactor(pos);
	vec4 tempcolor = texture(tex0, texCoord) * ambient + vec4(0, 0, 0, 0.41f);

	FragColor = mix(vec4(vec3(1.0f), 1.0f), tempcolor, fogfactor) + min(exp(pos.y * 2), 1) / 20;
	
}