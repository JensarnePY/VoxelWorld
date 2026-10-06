#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include<vector>

#include"../Camera.h"
#include"Texture.h"

enum face
{
	top = 0,
	bottom = 1,
	right = 2,
	left = 3,
	back = 4,
	front = 5,
	none = 6
};

enum blockID : int
{
	air = 0,
	grass_block = 1,
	dirt_block = 2,
	defalt_stone = 3,
	wood_log = 4,
	leaves = 5,
	water = 6,
	sand = 7
};

struct Vertex
{
	float position[3];
	float face;
	float blockID;
};

struct WaterVertex
{
	float position[3];
	float face;
};

class Mesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <Texture> textures;

	GLuint EmptyVAOID = 0;
	GLuint SSBOvert = 0;

	Mesh() = default;
	const void makeMash(std::vector <Texture>& textures);
	void clear();
	void Draw(Shader& shader, Camera& camera);
};


class WaterMesh
{
public:
	std::vector <WaterVertex> vertices;
	std::vector <Texture> textures;

	GLuint EmptyVAOID = 0;
	GLuint SSBOvert = 0;

	WaterMesh() = default;
	const void makeMash(std::vector <Texture>& textures);
	void clear();
	void Draw(Shader& shader, Camera& camera);
};



#endif