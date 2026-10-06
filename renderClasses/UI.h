#pragma once
#include"../world.h"
#include"../camera.h"
#include"../Inventory.h"
#include <memory>

class renUI {
private:
	GLuint m_VAOID = 0;
	GLuint m_VBOID = 0;
	GLuint m_EBOID = 0;
	Texture m_textures;

	int win_width = 0;
	int win_height = 0;

	glm::mat4 projection = glm::mat4(0.0f);
	glm::mat4 model = glm::mat4(1.0f);

	
public:

	glm::vec2 pos = glm::vec2(0.0f);
	glm::vec2 size = glm::vec2(0.0f);
	float rotate = 0.0f;

	renUI() = default;

	void toGPU(Texture& textures);
	void change_texture(Texture& textures);
	void update(int win_width, int win_height, glm::vec2 pos, glm::vec2 size, float rotate);
	void render(Shader& shader);
};


class UI {
private:
	renUI m_crossair;
	std::vector<renUI> m_render_hotbar;
	std::vector<renUI> m_render_hotbar_item;
	GLFWwindow* m_window;

	int m_hotbar_selected = 1;

	std::vector<Texture> m_textures{
			Texture("res/tex/no_img.png",       0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/grass_side.png",   0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/grass_bottom.png", 0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/stone.png",        0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/log.png",          0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/leaves.png",       0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/water.png",        0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("res/tex/sand.png",         0, GL_RGBA, GL_UNSIGNED_BYTE),
	};
public:
	UI(GLFWwindow* window);

	void update(Inventory &inventory);
	void render(Shader& shader);
};