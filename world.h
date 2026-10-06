#pragma once
#include <future>
#include <thread>
#include "renderClasses/Mesh.h"
#include "FastNoiseLite.h"
#include "chunk.h"

class chunkdata;

struct raycastRES
{
	glm::vec3 pos;
	chunkdata* chunk;
	bool hit;
	int i;
};

class world {
private:
	int m_threads_gen = 0;
	int m_MAX_threads_gen = 24;
	const int chunkSize = 32;
	const int m_render_distent = 50 * chunkSize;
public:
	
	std::vector<Texture> textures{
			Texture("res/tex/grass_top.png",    0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/grass_bottom.png", 1, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/grass_side.png",   2, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/stone.png",        3, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/log.png",          4, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/leaves.png",       5, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/water.png",        6, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/sand.png",         7, GL_RGBA, GL_UNSIGNED_BYTE),
	};

	std::vector<Texture> watertextures{
			Texture("res/tex/water.png", 0, GL_RGBA, GL_UNSIGNED_BYTE),
	};

	FastNoiseLite Noise;
	bool load_terain = true;
	std::vector<chunkdata> chunk;
	glm::vec3 player_chunk = glm::vec3(0.0f);
	world();
	raycastRES raycast(const glm::vec3 start, const glm::vec3 dir, const float max_distan, bool the_hit_bafore);
	chunkdata* getchunk(int x, int y, int z);
	void print_vec3(glm::vec3 vec3);
	void pre_load_chunk(glm::vec3 pos, int renderDistent);
	void update(Camera& camera, int renderDistent);
	void render(Shader& shader, Shader& watershader, Camera& camera);
};