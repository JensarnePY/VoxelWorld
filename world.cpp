#include "world.h"

void world::print_vec3(glm::vec3 vec3) {
	std::cout << vec3.x << ", " << vec3.y << ", " << vec3.z << ", \n";
}

chunkdata* world::getchunk(int x, int y, int z) {
	for (chunkdata& ch : chunk) {
		if (ch.pos == glm::vec3(x, y, z)) return &ch;
		
	}
	return nullptr;
}

static bool chunkExists(const std::vector<chunkdata>& chunks, const glm::vec3& pos){
	return std::any_of(chunks.begin(), chunks.end(),
		[&pos](const chunkdata& ch) { return ch.pos == pos; });
}

void gen_q(std::vector<chunkdata>* genqueue, const FastNoiseLite* Noise, bool* gen_complete) {
	std::cout << "ingame gen start Thread ID: " << std::this_thread::get_id() << "\n";

	while (1) {
		if (*gen_complete == false) {
			for (auto& ch : *genqueue) {
				ch.gen_chunkdata(Noise);
			}
			*gen_complete = true;
		}

	}

	std::cout << "ingame gen end Thread ID: " << std::this_thread::get_id() << "\n";
}

world::world() {

	for (int x = -m_render_distent; x <= m_render_distent; x++) {
		for (int y = -m_render_distent / 2; y <= m_render_distent / 2; y++) {
			for (int z = -m_render_distent; z <= m_render_distent; z++) {

				glm::vec3 newChunkPos((player_chunk.x + x) * chunkSize, (player_chunk.y + y) * chunkSize, (player_chunk.z + z) * chunkSize);

				in_render_distent_pos.push_back(newChunkPos);
			}
		}
	}

	std::sort(chunk.begin(), chunk.end(), [&](const chunkdata& a, const chunkdata& b) {
		return glm::distance(glm::vec3(0.0f), a.pos) < glm::distance(glm::vec3(0.0f), b.pos);
		});

	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetSeed(1337);
	Noise.SetFrequency(0.01f);

	Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	Noise.SetFractalOctaves(4);
	Noise.SetFractalLacunarity(3.0f);
	Noise.SetFractalGain(0.5f);
	Noise.SetFractalWeightedStrength(1.0f);
	BLtexture.init();
	gen_thread = std::thread(&gen_q, &genqueue, &Noise, &gen_complete);
}
world::~world() {
	//if (gen_thread.joinable()) {
	//	gen_thread.join();
	//	gen_thread.~thread();
	//}

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

void world::gen() {
	for (auto& ch : chunk) {
		ch.gen_chunkdata(&Noise);
	}
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
	//std::cout << "mainThread ID: " << std::this_thread::get_id() << "\n";

	for (auto& pos : ChunkToLoad) {
		chunk.emplace_back(glm::vec3(pos));
	}

	std::thread t1(&world::gen, this);
	t1.join();


	// load the mesh to the GPU
	for (auto& chunk : chunk) {
		if (chunk.mesh.vertices.size() != 0) {
			chunk.mesh.makeMash();
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

void world::update(Camera& camera) {

	player_last_chunk = player_chunk;
	player_chunk = glm::ivec3(camera.Position / glm::vec3(chunkSize));

	if (gen_complete && Enable_terrain_gen) {

		for (auto& pos : genqueue) {

			if (pos.mesh.vertices.size() != 0) {
				//std::cout << "e\n";
				pos.mesh.makeMash();
				pos.render = true;
				pos.loaded_to_gpu = true;
			}
			if (pos.watermesh.vertices.size() != 0) {
				pos.watermesh.makeMash(watertextures);
				pos.waterrender = true;
				pos.water_loaded_to_gpu = true;
			}
			if (pos.mesh.vertices.size() == 0 && pos.watermesh.vertices.size() == 0) {
				empty_chunk.emplace_back(pos);
			}
			else {
				chunk.emplace_back(pos);
			}
		}
		genqueue.clear();

		//if (player_chunk == player_last_chunk) {
			for (int x = -m_render_distent; x <= m_render_distent; x++) {
				for (int y = -m_render_distent / 2; y <= m_render_distent / 2; y++) {
					for (int z = -m_render_distent; z <= m_render_distent; z++) {

						glm::vec3 newChunkPos((player_chunk.x + x) * chunkSize, (player_chunk.y + y) * chunkSize, (player_chunk.z + z) * chunkSize);

						if (!chunkExists(chunk, newChunkPos) && !chunkExists(empty_chunk, newChunkPos) && genqueue.size() <= m_MAX_threads_gen && glm::distance(camera.Position, newChunkPos) < m_render_distent * chunkSize) {
							genqueue.push_back(newChunkPos);
						}
					}
				}
			}
		//}
		gen_complete = false;
	}
	//std::sort(chunk.begin(), chunk.end(), [&](const chunkdata& a, const chunkdata& b) {
	//	return glm::distance(camera.Position, a.pos) < glm::distance(camera.Position, b.pos);
	//	});
	
	// load the mesh to the GPU
	for (int i = 0; i < chunk.size(); i++) {
		if (glm::distance(camera.Position, chunk[i].pos) > m_render_distent * chunkSize) {
			chunk.erase(chunk.begin() + i);
			return;
		}
		i++;
	}
	for (int i = 0; i < empty_chunk.size(); i++) {
		if (glm::distance(camera.Position, empty_chunk[i].pos) > m_render_distent * chunkSize) {
			empty_chunk.erase(empty_chunk.begin() + i);
			return;
		}
		i++;
	}
}

int world::render(Shader& shader, Shader& watershader, Camera& camera) {
	int i = 0;
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	shader.Activate();
	BLtexture.bindSSBO();

	for (auto& chunk : chunk) {
		if (chunk.render) {
			chunk.mesh.Draw();
			i++;
		}
	}
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	watershader.Activate();

	for (unsigned int i = 0; i < watertextures.size(); i++) {
		watertextures[i].texUnit(watershader, ("tex" + std::to_string(i)).c_str(), i);
		watertextures[i].Bind();
	}

	for (auto& chunk : chunk) {
		if (chunk.waterrender) {
			chunk.watermesh.Draw();
			i++;
		}
	}
	return i;
}