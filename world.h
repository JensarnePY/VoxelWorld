#pragma once
#include <algorithm>
#include <thread>
#include "renderClasses/Mesh.h"
#include <chrono>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "FastNoiseLite.h"
#include "chunk.h"
#include "camera.h"
#include "renderClasses/BLtex.h"

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
	const int m_MAX_threads_gen = 1000;
	const int chunkSize = 64;
	const int m_render_distent = 15;
	bool gen_complete = true;
	std::vector<glm::vec3> in_render_distent_pos;
public:
	
	BLtex BLtexture;
	std::vector<Texture> watertextures{
		Texture("res/tex/water.png", 0, GL_RGBA, GL_UNSIGNED_BYTE),
	};

	bool Enable_terrain_gen = true;
	FastNoiseLite Noise;
	std::vector<chunkdata> genqueue;
	std::vector<chunkdata> chunk;
	std::vector<chunkdata> empty_chunk;
	glm::vec3 player_chunk = glm::vec3(0.0f);
	glm::vec3 player_last_chunk = glm::vec3(0.0f);
	std::thread gen_thread;
	world();
	~world();
	void gen();
	raycastRES raycast(const glm::vec3 start, const glm::vec3 dir, const float max_distan, bool the_hit_bafore);
	chunkdata* getchunk(int x, int y, int z);
	void print_vec3(glm::vec3 vec3);
	void pre_load_chunk(glm::vec3 pos, int renderDistent);
	void update(Camera& camera);
	int render(Shader& shader, Shader& watershader, Camera& camera);
};