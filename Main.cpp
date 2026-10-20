
#include"renderClasses/Mesh.h"
#include "renderClasses/UI.h"
#include "renderClasses/shadowMapFBO.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
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
	inputhandler input;
	input.define_keys();
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

	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//ImGui::SetCurrentContext();
	while (!glfwWindowShouldClose(window))
	{
		double dt = glfwGetTime() - time;
		time = glfwGetTime();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		glfwPollEvents();
		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		

		input.update_keys(window);

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


		camera.Inputs(window, input, dt);
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

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	defaultshader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}