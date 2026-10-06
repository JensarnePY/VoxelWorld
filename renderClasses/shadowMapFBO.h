#pragma once

#include"Mesh.h"

class ShadowMapFBO{
public:

    ShadowMapFBO();
    ~ShadowMapFBO();
    bool Init(unsigned int size);
    void BindForWriting();
    void BindForReading(GLenum TextureUnit);

private:

    unsigned int m_size = 0;
    GLuint m_fbo;
    GLuint m_shadowMap;
};

