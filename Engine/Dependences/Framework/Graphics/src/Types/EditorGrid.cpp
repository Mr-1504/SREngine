//
// Created by Nikita on 28.01.2021.
//

#include <Types/EditorGrid.h>
#include <Render/Render.h>
#include <Render/Shader.h>
#include <Render/Camera.h>
#include <Debug.h>
#include <Events/EventManager.h>

Framework::Graphics::EditorGrid::EditorGrid(const std::string &shaderName, Render *render) : m_env(Environment::Get()) {
    this->m_render = render;
    this->m_shader = new Shader(this->m_render, shaderName);
}

Framework::Graphics::EditorGrid::~EditorGrid() {
    this->m_render = nullptr;
}

void Framework::Graphics::EditorGrid::Draw() {
    if (m_hasErrors)
        return;
    else if (!m_isCalculated) {
        m_hasErrors = !Calculate();
        if (m_hasErrors) {
            Helper::EventManager::Push(EventManager::Event::Fatal);
            return;
        }
    }

    if (this->m_shader->Use()) {
        Camera *camera = m_render->GetCurrentCamera();

        camera->UpdateShaderProjView(m_shader);
        m_shader->SetVec3("CamPos", camera->GetGLPosition());

        if (VAO)
            this->m_env->DrawTriangles(VAO, 6);

        //this->m_env->DrawInstancedVertices(VAO, IBO, 6);
    } else {
        //Helper::EventManager::Push(Helper::EventManager::Event::Fatal);
        this->m_hasErrors = true;
    }
}

Framework::Graphics::EditorGrid* Framework::Graphics::EditorGrid::Create(const std::string &shaderName, Framework::Graphics::Render *render) {
    Helper::Debug::Graph("EditorGrid::Create() : create new grid...");
    return new EditorGrid(shaderName, render);
}

void Framework::Graphics::EditorGrid::Free() {
    Helper::Debug::Graph("EditorGrid::Free() : free grid class pointer and free video memory...");

    if (this->m_shader)
        this->m_shader->Free();

    if (VAO)
        this->m_env->FreeMesh(VAO);

    delete this;
}

bool Framework::Graphics::EditorGrid::Calculate() {
    Debug::Graph("EditorGrid::Calculate() : calculating grid...");

    if (!this->m_env->CalculateEmptyVAO(VAO)) {
        Debug::Error("EditorGrid::Calculate() : failed calculate grid VAO!");
        return false;
    }

    this->m_isCalculated = true;

    return true;
}

