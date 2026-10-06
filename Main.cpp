
#include"renderClasses/Mesh.h"
#include "renderClasses/UI.h"
#include "renderClasses/shadowMapFBO.h"

#include "world.h"
#include "chunk.h"
#include "Inventory.h"
#include "collision.h"

int width = 1200;
int height = 800;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, width, height);


	Shader defaultshader("res/shader/default.vert", "res/shader/default.frag");
	Shader watershader(  "res/shader/water.vert",   "res/shader/water.frag");
	Shader uishader(     "res/shader/UI.vert",      "res/shader/UI.frag");
	glm::vec3 worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 worldModel = glm::mat4(1.0f);
	worldModel = glm::translate(worldModel, worldPos);

	defaultshader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(defaultshader.ID, "model"), 1, GL_FALSE, glm::value_ptr(worldModel));
	watershader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(watershader.ID, "model"), 1, GL_FALSE, glm::value_ptr(worldModel));


	/*
	5 chunks    650ms
	10 chunks   5100ms
	20 chunks  38200ms
	40 chunks N/Ams
	to pre load
	*/
	world World;
	World.pre_load_chunk(glm::vec3(0.0f), 5);
	Camera camera(width, height, glm::vec3(0.0f, 100.0f, 0.0f));
	collision collision;
	Inventory inventory(window);
	UI ui(window);
	ShadowMapFBO shadowmap;
	shadowmap.Init(1048);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);

	int cn = 0;
	double time = glfwGetTime();
	double time2 = glfwGetTime();

	bool lastmou_right = GLFW_RELEASE;
	bool lastmou_left = GLFW_RELEASE;

	while (!glfwWindowShouldClose(window))
	{
		cn++;
		double timediff = glfwGetTime() - time2;
		double dt = glfwGetTime() - time;
		time = glfwGetTime();
		if (glfwGetTime() - time2 >= 0.2)
		{
			std::string FPS = std::to_string((1.0 / timediff) * cn);
			std::string ms = std::to_string((timediff / cn) * 1000);
			std::string newTitle = FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			time2 = glfwGetTime();
			cn = 0;
			glfwGetWindowSize(window, &width, &height);
			glViewport(0, 0, width, height);
			camera.setWinSize(width, height);
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && lastmou_right == GLFW_RELEASE) {
			lastmou_right = GLFW_PRESS;
			raycastRES result = World.raycast(camera.Position, camera.Orientation, 50.0f, false);
			if (result.hit) {

				blockID block = (blockID)result.chunk->blockIdList[result.chunk->getpos(result.pos.x, result.pos.y, result.pos.z)];
				if (inventory.hotbarslot[inventory.hotbar_selected].block == blockID::air) {
					inventory.hotbarslot[inventory.hotbar_selected].block = block;
				}
				if (inventory.hotbarslot[inventory.hotbar_selected].block == block) {
					inventory.hotbarslot[inventory.hotbar_selected].Count++;
				}
				
				result.chunk->blockIdList[result.chunk->getpos(result.pos.x, result.pos.y, result.pos.z)] = blockID::air;

				std::vector<bool> side = result.chunk->isAtCorner(result.pos);
				glm::vec3 chunkPos = result.chunk->pos;

				if (side[0]) {
					chunkdata* temp_chunk = World.getchunk(chunkPos.x, chunkPos.y + result.chunk->chunkSize, chunkPos.z);
					temp_chunk->addBlock(result.pos.x, -1, result.pos.z, blockID::air);
				}
				if (side[1]) {
					chunkdata* temp_chunk = World.getchunk(chunkPos.x, chunkPos.y - result.chunk->chunkSize, chunkPos.z);
					temp_chunk->addBlock(result.pos.x, 32, result.pos.z, blockID::air);
				}
				if (side[2]) {
					chunkdata* temp_chunk = World.getchunk(chunkPos.x + result.chunk->chunkSize, chunkPos.y, chunkPos.z);
					temp_chunk->addBlock(-1, result.pos.y, result.pos.z, blockID::air);
				}
				if (side[3]) {
					chunkdata* temp_chunk = World.getchunk(chunkPos.x - result.chunk->chunkSize, chunkPos.y, chunkPos.z);
					temp_chunk->addBlock(32, result.pos.y, result.pos.z, blockID::air);
				}
				if (side[4]) {
					chunkdata* temp_chunk = World.getchunk(chunkPos.x, chunkPos.y, chunkPos.z + result.chunk->chunkSize);
					temp_chunk->addBlock(result.pos.x, result.pos.y, -1, blockID::air);
				}
				if (side[5]) {
					chunkdata* temp_chunk = World.getchunk(chunkPos.x, chunkPos.y, chunkPos.z - result.chunk->chunkSize);
					temp_chunk->addBlock(result.pos.x, result.pos.y, 32, blockID::air);
				}
				result.chunk->reload();
			}
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) lastmou_right = GLFW_RELEASE;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && lastmou_left == GLFW_RELEASE) {
			lastmou_left = GLFW_PRESS;
			raycastRES result = World.raycast(camera.Position, camera.Orientation, 50.0f, true);
			if (result.hit && inventory.hotbarslot[inventory.hotbar_selected].Count > 0) {

				inventory.hotbarslot[inventory.hotbar_selected].Count--;
				result.chunk->blockIdList[result.chunk->getpos(result.pos.x, result.pos.y, result.pos.z)] = inventory.hotbarslot[inventory.hotbar_selected].block;
				result.chunk->reload();
			}
			if (result.hit && inventory.hotbarslot[inventory.hotbar_selected].Count == 0) {
				inventory.hotbarslot[inventory.hotbar_selected].block = blockID::air;
			}
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) lastmou_left = GLFW_RELEASE;


		if (camera.exit()) return 0;


		camera.Inputs(window, dt);
		camera.updateMatrix(70.0f, 0.1f, 10000.0f);

		defaultshader.Activate();
		glUniform1f(glGetUniformLocation(defaultshader.ID, "time"), (float)glfwGetTime());
		watershader.Activate();
		glUniform1f(glGetUniformLocation(watershader.ID, "time"), (float)glfwGetTime());
		glUniform3f(glGetUniformLocation(watershader.ID, "camDir"), camera.Orientation.x,
			camera.Orientation.y,
			camera.Orientation.z);

		//update all
		//__________________________________

		World.update(camera, 10);
		inventory.update();
		collision.player_collision(camera, World);
		ui.update(inventory);

		// shadowpass & render
		//_______________________________

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		World.render(defaultshader, watershader, camera);
		//


		// render ui
		//__________________________________
		ui.render(uishader);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	defaultshader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}