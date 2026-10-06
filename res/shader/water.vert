#version 460 core

struct WaterVertex
{
	float position[3];
	float face;
};

layout(binding = 0, std430) readonly buffer ssboVertices {
    WaterVertex data[];
};

uniform mat4 camMatrix;
uniform mat4 model;
uniform vec3 camPos;
uniform float time;

out vec3 pos;
out vec3 normal;
out vec2 texCoord;
out float face;


vec3 getPos(int index) {
    return vec3(data[index].position[0],
                data[index].position[1],
                data[index].position[2]);
}


const vec2 rawtexUV[4] = vec2[4]
(
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 1.0f)
);

const vec2 rewrawtexUV[4] = vec2[4]
(
	vec2(0.0f, 1.0f),
	vec2(0.0f, 0.0f),
	vec2(1.0f, 1.0f),
	vec2(1.0f, 0.0f)
);

int indices[6] = {0, 3, 1, 0, 2, 3};
int iindices[6] = {3,2,0,1,3,0};

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f*f*(3.0-2.0*f);

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}


void main()
{
    const int index = gl_VertexID / 6;
    const int currVertexID = gl_VertexID % 6;


    face     = data[index].face;
    pos = getPos(index);

    if(face == 0){

		const vec3 topfacePos[4] = vec3[4]
		(
			vec3(0.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(0.0f, 1.0f, 0.0f),
			vec3(1.0f, 1.0f, 0.0f)
		);
		pos += topfacePos   [iindices[currVertexID]];

		float utime = time / 2;
		pos.y =+ (sin(noise(vec2(utime + pos.x / 4, utime + pos.z / 4)))) - 0.8f;
		
		texCoord = rawtexUV[indices[currVertexID]];
		normal = vec3(0.0f, 1.0f, 0.0f);
	}
    else if(face == 1){

		const vec3 bottomfacePos[4] = vec3[4]
		(
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 1.0f)
		);

		pos += bottomfacePos[indices[currVertexID]];
		texCoord = rewrawtexUV[indices[currVertexID]];
		normal = vec3(0.0f, -1.0f, 0.0f);
	} 
    else if(face == 2){

		const vec3 rightfacePos[4] = vec3[4]
		(
			vec3(1.0f, 0.0f, 1.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 0.0f)
		);

		pos += rightfacePos [indices[currVertexID]];
		texCoord = rawtexUV[indices[currVertexID]];
		normal = vec3(1.0f, 0.0f, 0.0f);
	} 
    else if(face == 3){

		const vec3 leftfacePos[4] = vec3[4]
		(
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f)
		);

		pos += leftfacePos  [indices[currVertexID]];
		texCoord = rewrawtexUV[indices[currVertexID]];
		normal = vec3(-1.0f, 0.0f, 0.0f);
	} 
    else if(face == 4){

		const vec3 backfacePos[4] = vec3[4]
		(
			vec3(0.0f, 1.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 0.0f, 1.0f)
		);

		pos += backfacePos  [indices[currVertexID]];
		texCoord = rewrawtexUV[indices[currVertexID]];
		normal = vec3(0.0f, 0.0f, 1.0f);
	} 
    else if(face == 5){
		
		const vec3 frontfacePos[4] = vec3[4]
		(
			vec3(1.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(1.0f, 1.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f)
		);

		pos += frontfacePos [indices[currVertexID]];
		texCoord = rawtexUV[indices[currVertexID]];
		normal = vec3(0.0f, 0.0f, -1.0f);
	}

    vec3 worldPos = vec3(model * vec4(pos, 1.0));

    gl_Position = camMatrix * vec4(worldPos, 1.0);
}