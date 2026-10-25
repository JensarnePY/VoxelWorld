#pragma once

#include<glad/glad.h>
#include <vector>
#include<stb/stb_image.h>

#include"shaderClass.h"
#include"Texture.h"

class BLtex
{
private:
	std::vector<GLuint64> texHandles;
	std::vector<unsigned int> ID;
	GLuint SSBOhandle;

public:
	//BLtex();
	~BLtex();
	void init();
	void bindSSBO();
	void makeNonResident();
};

