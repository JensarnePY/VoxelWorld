
#include "world.h"

chunkdata* world::getchunk(int x, int y, int z) {
	for (chunkdata& ch : chunk) {
		if (ch.pos == glm::vec3(x, y, z)) return &ch;
		
	}
	return nullptr;
}

static bool chunkExists(const std::vector<chunkdata>& chunks, const glm::vec3& pos)
{
	return std::any_of(chunks.begin(), chunks.end(),
		[&pos](const chunkdata& ch) { return ch.pos == pos; });
}

world::world() {

	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetSeed(1337);
	Noise.SetFrequency(0.01f);

	Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	Noise.SetFractalOctaves(3);
	Noise.SetFractalLacunarity(3.0f);
	Noise.SetFractalGain(0.5f);
	Noise.SetFractalWeightedStrength(1.0f);
	
}

raycastRES world::raycast(const glm::vec3 start, const glm::vec3 dir, const float max_distan, bool the_hit_bafore) {

	float curr_distan = 0.0f;

	raycastRES last_hit{ glm::vec3(0.0f), nullptr, false, 0 };

	while (curr_distan < max_distan) {

		curr_distan += 0.02f;

		glm::vec3 res_pos = start + dir * curr_distan;

		int chunkX = res_pos.x >= 0 ? res_pos.x / 32 : res_pos.x / 32 - 1;
		int chunkY = res_pos.y >= 0 ? res_pos.y / 32 : res_pos.y / 32 - 1;
		int chunkZ = res_pos.z >= 0 ? res_pos.z / 32 : res_pos.z / 32 - 1;
		chunkdata* chunk = getchunk(chunkX * chunkSize, chunkY * chunkSize, chunkZ * chunkSize);
		if (chunk == nullptr) continue;
		if (chunk->render == false && chunk->waterrender == false) continue;

		int localBlockX = ((int)floor(res_pos.x) % 32 + 32) % 32;
		int localBlockY = ((int)floor(res_pos.y) % 32 + 32) % 32;
		int localBlockZ = ((int)floor(res_pos.z) % 32 + 32) % 32;

		glm::vec3 pos(localBlockX, localBlockY, localBlockZ);
		int i = chunk->getpos(localBlockX, localBlockY, localBlockZ);
		if (!chunk->issafe(i)) continue;

		// Check if block is solid
		if (chunk->blockIdList[i] != blockID::air && chunk->waterList[i] != true) {
			if (the_hit_bafore == false) return { pos, chunk, true, i }; // hit
			else return last_hit;
		}
		last_hit = raycastRES{ pos, chunk, true, i };
	}

	return { glm::vec3(0.0f), nullptr, false, 0 };
}

static void gen_chunk(chunkdata* chunk, const FastNoiseLite* Noise) {
	chunk->gen_chunkdata(Noise);
}

void world::print_vec3(glm::vec3 vec3) {
	std::cout << vec3.x << ", " << vec3.y << ", " << vec3.z << ", \n";
}

void world::pre_load_chunk(glm::vec3 pos, int renderDistent) {
	auto start = glfwGetTime();

	std::vector<glm::vec3>ChunkToLoad;
	int current_chunkX = static_cast<int>(std::floor(pos.x / chunkSize));
	int current_chunkY = static_cast<int>(std::floor(pos.y / chunkSize));
	int current_chunkZ = static_cast<int>(std::floor(pos.z / chunkSize));
	for (int x = -renderDistent; x <= renderDistent; x++) {
		for (int y = -renderDistent / 2; y <= renderDistent / 2; y++) {
			for (int z = -renderDistent; z <= renderDistent; z++) {

				ChunkToLoad.push_back(glm::vec3(
					(current_chunkX + x) * chunkSize,
					(current_chunkY + y) * chunkSize,
					(current_chunkZ + z) * chunkSize));
			}
		}
	}

	std::cout << ChunkToLoad.size() << " ChunkToLoad\n";

	// start to load the mesh in threads
	for (auto& pos : ChunkToLoad) {
		chunk.emplace_back(glm::vec3(pos));
		chunkdata& chunk_ref = chunk.back();
		std::future<void> mesh_fut = std::async(std::launch::async, gen_chunk, &chunk_ref, &Noise);
	}

	// load the mesh to the GPU
	for (auto& chunk : chunk) {
		if (chunk.mesh.vertices.size() != 0) {
			chunk.mesh.makeMash(textures);
			chunk.render = true;
			chunk.loaded_to_gpu = true;
		}
		if (chunk.watermesh.vertices.size() != 0) {
			chunk.watermesh.makeMash(watertextures);
			chunk.waterrender = true;
			chunk.water_loaded_to_gpu = true;
		}
	}
	std::cout << (glfwGetTime() - start) * 1000 << "ms to preload\n";
	std::cout << ((glfwGetTime() - start) * 1000) / ChunkToLoad.size() << "ms per chunk\n";
}

void world::update(Camera& camera, int renderDistent) {

	if (load_terain == true) {
		//auto start = glfwGetTime();
		std::vector<glm::vec3>ChunkToLoad;
		int current_chunkX = std::floor(camera.Position.x / chunkSize);
		int current_chunkY = std::floor(camera.Position.y / chunkSize);
		int current_chunkZ = std::floor(camera.Position.z / chunkSize);

		for (int x = -renderDistent; x <= renderDistent; x++) {
			for (int y = -renderDistent / 2; y <= renderDistent / 2; y++) {
				for (int z = -renderDistent; z <= renderDistent; z++) {

					glm::vec3 newChunkPos(
						(current_chunkX + x) * chunkSize,
						(current_chunkY + y) * chunkSize,
						(current_chunkZ + z) * chunkSize);

					if (!chunkExists(chunk, newChunkPos) && ChunkToLoad.size() <= m_MAX_threads_gen / 2 && m_threads_gen < m_MAX_threads_gen) {
						ChunkToLoad.push_back(newChunkPos);
						m_threads_gen++;
					}
				}
			}
		}
		//std::cout << (glfwGetTime() - start) * 1000 << "ms \n";
		for (auto& pos : ChunkToLoad) {
			chunk.emplace_back(pos);
			chunkdata& chunk_ref = chunk.back();
			std::future<void> mesh_fut = std::async(std::launch::async, gen_chunk, &chunk_ref, &Noise);
			m_threads_gen--;
		}

		// load the mesh to the GPU
		for (auto& chunk : chunk) {

			if (glm::ivec3(camera.Position / glm::vec3(32)) == glm::ivec3(chunk.pos / glm::vec3(32))) {
				player_chunk = chunk.pos;
			}
			if (chunk.gen == false && glm::distance(camera.Position, chunk.pos) < m_render_distent) {
				chunk.reload();
			}
			if (glm::distance(camera.Position, chunk.pos) > m_render_distent) {
				chunk.gen = false;
				chunk.render = false;
				chunk.waterrender = false;
				chunk.loaded_to_gpu = false;
				chunk.water_loaded_to_gpu = false;
				chunk.mesh.clear();
				chunk.watermesh.clear();
			}
			if (chunk.gen == true && chunk.mesh.vertices.size() != 0 && chunk.loaded_to_gpu == false) {
				chunk.mesh.makeMash(textures);
				chunk.render = true;
				chunk.loaded_to_gpu = true;
			}
			if (chunk.gen == true && chunk.watermesh.vertices.size() != 0 && chunk.water_loaded_to_gpu == false) {
				chunk.watermesh.makeMash(watertextures);
				chunk.waterrender = true;
				chunk.water_loaded_to_gpu = true;
			}
		}
	}
}

void  world::render(Shader& shader, Shader& watershader, Camera& camera) {
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	for (auto& chunk : chunk) {
		if (chunk.render) {
			chunk.mesh.Draw(shader, camera);
		}
	}
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	for (auto& chunk : chunk) {
		if (chunk.waterrender) {
			chunk.watermesh.Draw(watershader, camera);
		}
	}
}