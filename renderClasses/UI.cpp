#include "UI.h"

UI::UI(GLFWwindow* window){
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    m_window = window;

    Texture crossair_textures ("res/tex/crossair.png", 0, GL_RGBA, GL_UNSIGNED_BYTE);
    m_crossair.toGPU(crossair_textures);
    m_crossair.update(width, height, glm::vec2(width / 2, height / 2), glm::vec2(20.0f), 0.0f);

    Texture hotbar_textures("res/tex/UI_inv.png", 0, GL_RGBA, GL_UNSIGNED_BYTE);
    m_render_hotbar.resize(9);
    int I = 0;
    for (auto& i : m_render_hotbar) {
        i.toGPU(hotbar_textures);
        i.update(width, height, glm::vec2(width / 2 - 320 + (I * 80), height - 70), glm::vec2(30.0f), 0.0f);
        I++;
    }

    m_render_hotbar_item.resize(9);
    I = 0;
    for (auto& i : m_render_hotbar_item) {
        i.toGPU(hotbar_textures);
        i.update(width, height, glm::vec2(width / 2 - 320 + (I * 80), height - 70), glm::vec2(30.0f), 0.0f);
        I++;
    }
}

void UI::update(Inventory &inventory) {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    m_crossair.update(width, height, glm::vec2(width / 2, height / 2), glm::vec2(20.0f), 0.0f);

    int I = 0;
    Texture hotbar_textures("res/tex/UI_inv.png", 0, GL_RGBA, GL_UNSIGNED_BYTE);
    Texture hotbar_selct_textures("res/tex/UI_inv_selct.png", 0, GL_RGBA, GL_UNSIGNED_BYTE);

    for (auto& i : m_render_hotbar) {
        
        i.change_texture(hotbar_textures);

        if (inventory.hotbar_selected == I) {
            
            i.change_texture(hotbar_selct_textures);
            
        }
        i.update(width, height, glm::vec2(width / 2 - 320 + (I * 80), height - 70), glm::vec2(30.0f), 0.0f);
        I++;
    }

    m_hotbar_selected = inventory.hotbar_selected;
    I = 0;
    for (auto& i : m_render_hotbar_item) {
        i.change_texture(m_textures[(int)inventory.hotbarslot[I].block]);
        i.update(width, height, glm::vec2(width / 2 - 320 + (I * 80), height - 70), glm::vec2(30.0f), 0.0f);
        I++;
    }
}

void UI::render(Shader& shader) {


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    m_crossair.render(shader);
    for (auto& u : m_render_hotbar_item) {
        u.render(shader);
    }
    for (auto &u : m_render_hotbar) {
        u.render(shader);
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}


void renUI::toGPU(Texture& textures) {

    GLfloat vertices[] = {
        // Positions     // TexCoords
        -1.0f, -1.0f,  0.0f, 0.0f, // Lower left
        -1.0f,  1.0f,  0.0f, 1.0f, // Upper left
         1.0f,  1.0f,  1.0f, 1.0f, // Upper right
         1.0f, -1.0f,  1.0f, 0.0f  // Lower right
    };


    GLuint indices[] = {
        0, 2, 1,
        0, 3, 2
    };

    m_textures = textures;

    glGenVertexArrays(1, &m_VAOID);
    glBindVertexArray(m_VAOID);

    // Vertex buffer
    glGenBuffers(1, &m_VBOID);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Element buffer
    glGenBuffers(1, &m_EBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // EBO stays bound to VAO
}

void renUI::change_texture(Texture& textures) {
    m_textures = textures;
}

void renUI::update(int win_width, int win_height, glm::vec2 pos, glm::vec2 size, float rotate) {
    this->pos = pos;
    this->size = size;
    this->rotate = rotate;

    this->win_width = win_width;
    this->win_height = win_height;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    projection = glm::ortho(0.0f, (float)win_width, (float)win_height, 0.0f, -1.0f, 1.0f);
}

void renUI::render(Shader& shader) {

	shader.Activate();
	glUniform1f(glGetUniformLocation(shader.ID, "scale"), 0.05f);
    shader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    shader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    m_textures.texUnit(shader, "tex0", 0);
    m_textures.Bind();

    
	glBindVertexArray(m_VAOID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}