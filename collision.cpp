#include "collision.h"


void collision::player_collision(Camera& camera, world& World) {

	player_box.update_pos(camera.Position);

	for (auto& ch : World.chunk) {
		if (ch.pos == World.player_chunk) {
			
			for (int z = 0; z < ch.chunkSize; z++) {
				for (int x = 0; x < ch.chunkSize; x++) {
					for (int y = 0; y < ch.chunkSize; y++) {
						if (ch.blockIdList[ch.getpos(x, y, z)] != blockID::air) {
							block_box.update_pos(x + ch.pos.x, y + ch.pos.y, z + ch.pos.z);
							if (player_box.intersect(block_box)) {
								camera.Position = camera.lastpos;
								//camera.Position -= glm::vec3(0, 0.1, 0);
								std::cout << "e";
							}
						}
					}
				}
			}
		}
	}
}