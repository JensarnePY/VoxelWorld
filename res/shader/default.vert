#version 460 core

struct Vertex {
    float position[3];
    float face;
    float blockID;
};

layout(binding = 0, std430) readonly buffer ssboVertices {
    Vertex data[];
};

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 lightWVP;
uniform float time;

out vec4 FragPosLightSpace;
out vec3 pos;
out vec3 normal;
out vec2 texCoord;
out float face;
out float blockID;


vec3 getPos(int index) {
    return vec3(data[index].position[0],
                data[index].position[1],
                data[index].position[2]);
}


const vec2 texUV[4] = vec2[4]
(
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 1.0f)
);

int indices[6] = {0, 3, 1, 0, 2, 3};


void main()
{
    const int index = gl_VertexID / 6;
    const int currVertexID = gl_VertexID % 6;


    blockID  = data[index].blockID;
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

		pos += topfacePos   [indices[currVertexID]];
		texCoord = texUV[indices[currVertexID]];
		normal = vec3(0.0f, 1.0f, 0.0f);
	}
    else if(face == 1){

		const vec3 bottomfacePos[4] = vec3[4]
		(
			vec3(1.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f)
		);

		pos += bottomfacePos[indices[currVertexID]];
		texCoord = texUV[indices[currVertexID]];
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
		texCoord = texUV[indices[currVertexID]];
		normal = vec3(1.0f, 0.0f, 0.0f);
	} 
    else if(face == 3){

		const vec3 leftfacePos[4] = vec3[4]
		(
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, 1.0f, 1.0f)
		);

		pos += leftfacePos  [indices[currVertexID]];
		texCoord = texUV[indices[currVertexID]];
		normal = vec3(-1.0f, 0.0f, 0.0f);
	} 
    else if(face == 4){

		const vec3 backfacePos[4] = vec3[4]
		(
			vec3(0.0f, 0.0f, 1.0f),
			vec3(1.0f, 0.0f, 1.0f),
			vec3(0.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f)
		);

		pos += backfacePos  [indices[currVertexID]];
		texCoord = texUV[indices[currVertexID]];
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
		texCoord = texUV[indices[currVertexID]];
		normal = vec3(0.0f, 0.0f, -1.0f);
	}

    vec3 worldPos = vec3(model * vec4(pos, 1.0));
	FragPosLightSpace = lightWVP * vec4(worldPos, 1.0);
    gl_Position = camMatrix * vec4(worldPos, 1.0);
}