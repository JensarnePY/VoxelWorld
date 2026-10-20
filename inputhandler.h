#pragma once
#include <vector>
#include <GLFW/glfw3.h>

struct inputdata
{
	short key;
	bool state;
	bool beforestate;
};

class inputhandler
{
public:
	std::vector<inputdata> m_keys;

	void define_keys();
	void update_keys(GLFWwindow* window);
	bool get_key_state(short key);
	bool get_key_beforestate(short m_key);
	bool is_key_pressed(GLFWwindow* window, int key);
	bool is_key_pressestones(GLFWwindow* window, int key);
private:

};