//
// Created by Nikita on 17.11.2020.
//

#include "Types/Mesh.h"
#include <ResourceManager/ResourceManager.h>
#include <Render/Render.h>
#include <Render/Shader.h>
#include <Types/Material.h>
#include <Utils/StringUtils.h>
#include <Debug.h>
#include <exception>

#include <Loaders/ObjLoader.h>

#include <utility>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <map>

using namespace Framework::Graphics::Types;

inline static std::map<unsigned int, unsigned long> VAO_usages = std::map<unsigned int, unsigned long>();

Framework::Graphics::Types::Mesh::Mesh(Shader* shader, Material* material, std::string name)
    : IResource("Mesh"), m_env(Environment::Get()), Component("Mesh")
    {
    this->m_shader = shader;

    if (!m_shader)
        Debug::Error("Mesh::Constructor() : shader is nullptr!");

    this->m_geometry_name = std::move(name);
    this->m_material = material;

    if (!this->m_material)
        Debug::Warn("Mesh::Constructor() : material is nullptr! \n\tMesh name: "+m_geometry_name);
}

Framework::Graphics::Types::Mesh::~Mesh() {
    if (!m_material){
        Debug::Error("Mesh::~Mesh() : material is nullptr! Something went wrong...");
    } else{
        delete m_material;
        m_material = nullptr;
    }
}

bool Framework::Graphics::Types::Mesh::Destroy() {
    if (m_isDestroy)
        return false;

    this->m_isDestroy = true;

    Helper::ResourceManager::Destroy(this);

    return true;
}

std::vector<Mesh *> Framework::Graphics::Types::Mesh::Load(std::string path) {
#ifdef WIN32
    path = StringUtils::MakePath(path, true);
#else
    path = StringUtils::MakePath(path, false);
#endif
    std::vector<Mesh*> meshes = std::vector<Mesh*>();

    unsigned int counter = 0;
ret:
    IResource* find = ResourceManager::Find("Mesh", path + " - "+ std::to_string(counter));
    if (find) {
        Mesh* copy = ((Mesh*)(find))->Copy();
        if (!copy) {
            Debug::Error("Mesh::Load() : [FATAL] An unforeseen situation has arisen, apparently, it is necessary to work out this piece of code...");
            throw "This should never happen.";
        }

        meshes.push_back(copy);
        counter++;
        goto ret;
    } else
        if (counter > 0)
            return meshes;

    std::string ext = StringUtils::GetExtensionFromFilePath(path);

    if (ext == "obj"){
        std::string file = path;
        file.resize(path.size() - 4);
        meshes = ObjLoader::Load(file);
    } else if (ext == "fbx"){
        meshes = std::vector<Mesh *>();
    } else {
        Helper::Debug::Error("Mesh::Load() : unknown \""+ext+"\" format!");
        meshes = std::vector<Mesh*>();
    }

    for (unsigned short i = 0; i < meshes.size(); i++) {
        meshes[i]->m_resource_id = path + " - " + std::to_string(i);
    }

    return meshes;
}

void Mesh::ReCalcModel() {
    glm::mat4 modelMat = glm::mat4(1.0f);

    modelMat = glm::translate(modelMat, {
            //-m_position.z, m_position.y, -m_position.x
            m_position.x, m_position.y, m_position.z
             //0, -8, -25
    }); //

    modelMat = glm::rotate(modelMat, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    modelMat = glm::rotate(modelMat, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    modelMat = glm::rotate(modelMat, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));

    modelMat = glm::scale(modelMat, m_scale);

    this->m_modelMat = modelMat;
}

bool Mesh::Calculate() {
    m_mutex.lock();

    if (Debug::GetLevel() >= Debug::Level::High)
        Debug::Log("Mesh::Calculate() : calculating \""+ m_geometry_name +"\"...");

    unsigned int VBO = 0;
    if (!this->m_env->CalculateMesh(VBO, m_VAO, m_vertices, m_countVertices)) {
        Debug::Error("Mesh::Calculate() : failed calculate \"" + m_geometry_name + "\" mesh!");
        m_mutex.unlock();
        return false;
    }

    VAO_usages[m_VAO]++;

    m_isCalculated = true;

    m_mutex.unlock();

    return true;
}

Mesh *Mesh::Copy() {
    if (m_isDestroy) {
        Debug::Error("Mesh::Copy() : mesh already destroyed!");
        return nullptr;
    }

    if (Debug::GetLevel() >= Debug::Level::High)
        Debug::Log("Mesh::Copy() : copy \""+m_resource_id+ "\" mesh...");

    if (!m_material){
        Debug::Error("Mesh::Copy() : material is nullptr! Something went wrong...");
        return nullptr;
    }

    m_mutex.lock();

    Mesh* copy = new Mesh(this->m_shader, m_material->Copy(), this->m_geometry_name);
    copy->m_countVertices = m_countVertices;
    copy->m_position = m_position;
    copy->m_rotation = m_rotation;
    copy->m_scale = m_scale;
    if (m_isCalculated) {
        VAO_usages[m_VAO]++;
        copy->m_VAO = m_VAO;
    }else{
        copy->m_vertices = m_vertices;
    }
    copy->m_isCalculated = m_isCalculated;
    copy->m_render = m_render;
    copy->m_modelMat = m_modelMat;

    m_mutex.unlock();

    return copy;
}

bool Mesh::Draw() {
    if (m_isDestroy) return false;

    if (!m_isCalculated)
        this->Calculate();

    this->m_shader->SetMat4("modelMat", m_modelMat);
    this->m_shader->SetVec3("color", m_material->m_color);
    this->m_shader->SetInt("bloom", (int)m_material->m_bloom);

    this->m_env->DrawTriangles(m_VAO, m_countVertices);

    return true;
}

bool Mesh::FreeVideoMemory() {
    if (m_VAO > 0) {
        VAO_usages[m_VAO]--;
        if (VAO_usages[m_VAO] == 0)
            m_env->FreeMesh(m_VAO);
        m_isCalculated = false;
        return true;
    }
    else {
        Debug::Error("Mesh:FreeVideoMemory() : VAO is zero! Something went wrong...");
        return false;
    }
}

void Mesh::OnDestroyGameObject() noexcept {

}


