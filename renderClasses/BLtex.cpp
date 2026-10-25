#include "BLtex.h"

//BLtex::BLtex(){}

void BLtex::init() {
	std::vector<const char*> textureFiles = {
		"res/tex/grass_top.png",
		"res/tex/grass_bottom.png",
		"res/tex/grass_side.png",
		"res/tex/stone.png",
		"res/tex/log.png",
		"res/tex/leaves.png",
		"res/tex/water.png",
		"res/tex/sand.png"
	};

	texHandles.resize(textureFiles.size());
	ID.resize(textureFiles.size());
	stbi_set_flip_vertically_on_load(true);
	for (int i = 0; i < textureFiles.size(); i++) {

		int widthImg, heightImg, numColCh;
		unsigned char* bytes = stbi_load(textureFiles[i], &widthImg, &heightImg, &numColCh, 0);

		GLsizei levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(widthImg, heightImg))));

		glGenTextures(1, &ID[i]);
		glBindTexture(GL_TEXTURE_2D, ID[i]);
		glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, widthImg, heightImg); // num of mip
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, widthImg, heightImg, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(bytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		texHandles[i] = glGetTextureHandleARB(ID[i]);
		if (texHandles[i] == 0) {
			std::cerr << "Failed to get texture handle for texture: " << textureFiles[i] << std::endl;
		}
	}

	glCreateBuffers(1, &SSBOhandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOhandle);
	glNamedBufferStorage(SSBOhandle, texHandles.size() * sizeof(GLuint64), texHandles.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOhandle);

	for (const auto& handle : texHandles) {
		glMakeTextureHandleResidentARB(handle);
	}
}

BLtex::~BLtex() {
	makeNonResident();
	glDeleteBuffers(1, &SSBOhandle);
	glDeleteTextures(texHandles.size(), ID.data());
}

void BLtex::bindSSBO() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOhandle);
}

void BLtex::makeNonResident() {
	for (const auto& handle : texHandles) {
		glMakeTextureHandleNonResidentARB(handle);
	}
}