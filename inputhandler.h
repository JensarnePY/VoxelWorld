#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream>

struct inputdata
{
	short key;
	bool state = false;
	bool beforestate = false;
	bool switchstate = false;
};

class inputhandler
{
public:
	std::vector<inputdata> m_keys;
	bool e = false;
	void define_keys();
	void update_keys(GLFWwindow* window);
	bool get_key_state(short key);
	bool get_key_beforestate(short key);
	bool is_key_pressed(GLFWwindow* window, short key);
	bool is_key_pressestones(GLFWwindow* window, short key);
	bool switch_key(GLFWwindow* window, short key);
private:
};