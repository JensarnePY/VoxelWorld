#version 460 core
#extension GL_ARB_bindless_texture : require

layout (binding = 1, std430) readonly buffer texhandels{
	sampler2D tex[];
};

out vec4 FragColor;

in vec4 FragPosLightSpace;
in vec3 pos;
in vec3 normal;
in vec2 texCoord;
in float face;
in float blockID;

uniform vec3 camPos;
uniform vec3 camDir;
uniform float time;

//	grass_top = 0
//	grass_block = 1
//	dirt_block = 2
//	defalt_stone = 3
//	wood_log = 4
//	leaves = 5
//	water = 6
//	sand = 7
//

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
	if(texCoord.x < 1.0f || texCoord.y > 1.0f) {discard;}
	
	// diffuse lighting
	vec3 Normal = normalize(normal);
	vec3 lightDirection = normalize(vec3(1.5f, 1.0f, 1.25f));
	float diffuse = max(dot(Normal, lightDirection), 0.0f);
	
	float ambient = 0.4f;
	float addon = ambient + diffuse;
	vec4 tempcolor;

	if(blockID == 1){

		if(face == 0) tempcolor = texture(tex[0], texCoord) * addon;
		else if(face == 1) tempcolor = texture(tex[1], texCoord) * addon;
		else tempcolor = texture(tex[2], texCoord) * addon;

	}if(blockID == 2){

		tempcolor = texture(tex[1], texCoord) * addon;
	}if(blockID == 3){

		tempcolor = texture(tex[3], texCoord) * addon;
	}if(blockID == 4){

		tempcolor = texture(tex[4], texCoord) * addon;
	}if(blockID == 5){

		tempcolor = texture(tex[5], texCoord) * addon;
	}if(blockID == 6){

		tempcolor = texture(tex[6], texCoord) * addon;
	}if(blockID == 7){

		tempcolor = texture(tex[7], texCoord) * 0.6f * addon;
	}

	float fogfactor = CalcExpFogFactor(pos);
	tempcolor = mix(vec4(vec3(0.5f), 1.0f), tempcolor, fogfactor);

	FragColor = tempcolor;
}