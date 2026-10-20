#include "inputhandler.h"


void inputhandler::define_keys()
{
	m_keys.resize(348);
	for (int i = 0; i < 348; i++)
	{
		m_keys[i] = inputdata{ (short)i, false, false, false};
	}
}

void inputhandler::update_keys(GLFWwindow* window)
{
	for (int i = 0; i < 348; i++) {

		m_keys[i].beforestate = m_keys[i].state;
		m_keys[i].state = glfwGetKey(window, m_keys[i].key) == GLFW_PRESS;

	}
}

bool inputhandler::get_key_state(short key)
{
	return m_keys[key].state;
}

bool inputhandler::get_key_beforestate(short key)
{
	return m_keys[key].beforestate;
}

bool inputhandler::is_key_pressed(GLFWwindow* window, short key)
{
	bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
	m_keys[key].state = isPressed;
	return isPressed;
}

bool inputhandler::is_key_pressestones(GLFWwindow* window, short key)
{
	if (glfwGetKey(window, key) == GLFW_PRESS) {
		m_keys[key].state = true;
		if (m_keys[key].beforestate == false) { return true; }
	}
	return false;
}

bool inputhandler::switch_key(GLFWwindow* window, short key) {

	if (is_key_pressed(window, key)) {

		e = true;
	}
	std::cout << e << "\n";
	return e;

}