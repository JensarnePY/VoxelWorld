#include "Inventory.h"


Inventory::Inventory(GLFWwindow* window) {
	m_window = window;
	hotbarslot.resize(9);
}

item Inventory::hotbarslot_selected() {
	return hotbarslot[hotbar_selected];
}

void Inventory::update() {

	if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS) hotbar_selected = 0;
	if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS) hotbar_selected = 1;
	if (glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS) hotbar_selected = 2;
	if (glfwGetKey(m_window, GLFW_KEY_4) == GLFW_PRESS) hotbar_selected = 3;
	if (glfwGetKey(m_window, GLFW_KEY_5) == GLFW_PRESS) hotbar_selected = 4;
	if (glfwGetKey(m_window, GLFW_KEY_6) == GLFW_PRESS) hotbar_selected = 5;
	if (glfwGetKey(m_window, GLFW_KEY_7) == GLFW_PRESS) hotbar_selected = 6;
	if (glfwGetKey(m_window, GLFW_KEY_8) == GLFW_PRESS) hotbar_selected = 7;
	if (glfwGetKey(m_window, GLFW_KEY_9) == GLFW_PRESS) hotbar_selected = 8;
	
	for (auto& i : hotbarslot) {
		if (i.Count == 0) {
			i.block = blockID::air;
		}
	}

}


