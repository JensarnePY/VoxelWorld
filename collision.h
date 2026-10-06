#pragma once
#include<glm/glm.hpp>
#include"Camera.h"
#include"world.h"

struct hitbox 
{
	glm::vec3 pos;
	glm::vec3 baondbox;

	bool intersect(const hitbox &box) {
		return  (get_pos_NX() >= box.get_pos_X() && get_pos_X() <= box.get_pos_NX()) &&
			   	(get_pos_NY() >= box.get_pos_Y() && get_pos_Y() <= box.get_pos_NY()) &&
				(get_pos_NZ() >= box.get_pos_Z() && get_pos_Z() <= box.get_pos_NZ());
		 
		//return (get_pos_NY() >= box.get_pos_Y() && get_pos_Y() <= box.get_pos_NY());
	}

	void update_pos(glm::vec3 new_pos) { pos = new_pos; }
	void update_pos(int x, int y, int z) { pos = glm::vec3(x,y,z); }

	float get_pos_X() const  { return pos.x; };
	float get_pos_Y() const  { return pos.y; };
	float get_pos_Z() const  { return pos.z; };
	float get_pos_NX() const { return baondbox.x + pos.x; };
	float get_pos_NY() const { return baondbox.y + pos.y; };
	float get_pos_NZ() const { return baondbox.z + pos.z; };
};

class collision {
private:
	glm::vec3 player_hit_box = glm::vec3(0.8, 0.8, 0.8);
	glm::vec3 defalt_block_hit_box = glm::vec3(1.0f);
	hitbox player_box{ glm::vec3(0.0f), defalt_block_hit_box };
	hitbox block_box{ glm::vec3(0.0f), defalt_block_hit_box };

public:
	collision() = default;
	void player_collision(Camera &camera, world &World);


};

