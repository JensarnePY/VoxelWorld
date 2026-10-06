#pragma once
#include "renderClasses/Mesh.h"

struct item{
	blockID block = blockID::air;
	short Count = 0;
};

class Inventory{
private:
	GLFWwindow* m_window;
public:

	int hotbar_selected = 0;
	std::vector<item> hotbarslot;

	Inventory(GLFWwindow* window);
	item hotbarslot_selected();
	void update();
};
