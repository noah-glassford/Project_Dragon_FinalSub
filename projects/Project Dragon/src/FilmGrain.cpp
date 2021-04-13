#include "FilmGrain.h"
#include <chrono>
#include <GLFW/glfw3.h>
void FilmGrain::Init(unsigned width, unsigned height)
{
    int index = int(_buffers.size());
    _buffers.push_back(new Framebuffer());
    _buffers[index]->AddColorTarget(GL_RGBA8);
    _buffers[index]->AddDepthTarget();
    _buffers[index]->Init(width, height);

    //Set up shaders
    index = int(_shaders.size());
    _shaders.push_back(Shader::Create());
    _shaders[index]->LoadShaderPartFromFile("shader/passthrough_vert.glsl", GL_VERTEX_SHADER);
    _shaders[index]->LoadShaderPartFromFile("shader/filmgrain_frag.glsl", GL_FRAGMENT_SHADER);
    _shaders[index]->Link();

    PostEffect::Init(width, height);
}

void FilmGrain::ApplyEffect(PostEffect* buffer)
{
    BindShader(0);
    _shaders[0]->SetUniform("iResolution", glm::vec2(buffer->GetBuffer(0)->_width, buffer->GetBuffer(0)->_height));
    _shaders[0]->SetUniform("u_Intensity", m_Intensity);
    //time for randomization
    _shaders[0]->SetUniform("time",(float) glfwGetTime());
    buffer->BindColorAsTexture(0, 0, 0);

    _buffers[0]->RenderToFSQ();

    buffer->UnbindTexture(0);

    UnbindShader();
}
