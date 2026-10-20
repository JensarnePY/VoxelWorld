#pragma once

#include "world.h"
#include "math.h"


class chunkdata {
public:
	glm::vec3 pos;
	Mesh mesh;
	WaterMesh watermesh;
	bool gen = false;
	bool render = false;
	bool loaded_to_gpu = false;

	bool waterrender = false;
	bool water_loaded_to_gpu = false;
	std::vector <float> blockIdList;
	std::vector <bool> waterList;
	static constexpr int chunkSize = 32;

	chunkdata(glm::vec3 position) : pos(position) {}

	int getpos(int x, int y, int z)const;
	int getposno1(int x, int y, int z) const;
	int getpos(float x, float y, float z)const;
	int getpos(float x, float z)const;

	void reload();

	float noise(float x, float z, const FastNoiseLite* Noise)const;

	bool issafe(int i);
	std::vector<bool> isAtCorner(int x, int y, int z);
	std::vector<bool> isAtCorner(glm::vec3 pos);

	int pow2(const int num);
	int pow3(const int num);

	void addBlock(int x, int y, int z, blockID block);

	void make_noiselist(bool* is_nedad, const std::vector <float>& NoiseList, const std::vector <float>& NoiseList3D);
	void make_water_noiselist(bool* is_nedad, const std::vector <float>& NoiseList);

	void make_blockId(const FastNoiseLite* Noise);
	void make_tree(const FastNoiseLite* Noise);
	void make_vertices();
	void make_water_vertices();

	void gen_chunkdata(const FastNoiseLite* Noise);

};