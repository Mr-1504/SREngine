//
// Created by Monika on 26.07.2021.
//

#ifndef FBXLOADER_MESH_H
#define FBXLOADER_MESH_H

#include <vector>
#include <string>
#include <cctype>
#include <math.h>
#include "Debug.h"

namespace FbxLoader {
    template <class T> inline void hash_combine(std::size_t & s, const T & v) {
        std::hash<T> h;
        s^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }

    struct vec3 {
        float_t x, y, z;

        bool operator==(const vec3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    struct vec2 {
        float_t x, y;

        bool operator==(const vec2& other) const {
            return x == other.x && y == other.y;
        }
    };

    struct Vertex {
        vec3 pos;
        vec3 norm;
        vec3 binorm;
        vec3 tang;
        vec2 uv;

        bool operator==(const Vertex& other) const {
            return pos    == other.pos
                && norm   == other.norm
                && binorm == other.binorm
                && tang   == other.tang
                && uv     == other.uv;
        }
    };

    static std::vector<Vertex> MakeVertices(
            const std::vector<uint32_t>& indices,
            const std::vector<float_t>& positions,
            const std::vector<float_t>& normals,
            const std::vector<float_t>& binormals,
            const std::vector<float_t>& tangents,
            const std::vector<float_t>& uvs,
            const std::vector<uint32_t>& uvIndices) {
        if (!uvIndices.empty() && uvs.empty() || (!uvIndices.empty() && uvIndices.size() != indices.size())) {
            FBX_ERROR("FbxLoader::MakeVertices() : incorrect uvs!");
            return {};
        }

        auto vertices = std::vector<Vertex>();

        for (uint32_t i = 0; i < indices.size(); i++) {
            Vertex vertex = {
                    .pos = {
                         positions[3 * indices[i] + 0],
                         positions[3 * indices[i] + 1],
                         positions[3 * indices[i] + 2]
                     },
                    .norm = normals.empty() ? vec3 { 0.f, 0.f, 0.f } : vec3 {
                         normals[3 * indices[i] + 0],
                         normals[3 * indices[i] + 1],
                         normals[3 * indices[i] + 2]
                    },
                    .binorm = binormals.empty() ? vec3 { 0.f, 0.f, 0.f } : vec3 {
                         binormals[3 * indices[i] + 0],
                         binormals[3 * indices[i] + 1],
                         binormals[3 * indices[i] + 2]
                    },
                    .tang = tangents.empty() ? vec3 { 0.f, 0.f, 0.f } : vec3 {
                            tangents[3 * indices[i] + 0],
                            tangents[3 * indices[i] + 1],
                            tangents[3 * indices[i] + 2]
                    },
                    .uv = uvs.empty() ? vec2 { 0.f, 0.f } : uvIndices.empty() ? vec2 {
                                   uvs[2 * indices[i] + 0],
                            1.0f - uvs[2 * indices[i] + 1],
                    } : vec2 {
                            uvs[2 * uvIndices[i] + 0],
                            uvs[2 * uvIndices[i] + 1],
                    }
            };

            vertices.emplace_back(vertex);
        }

        return vertices;
    }

    inline static std::vector<uint32_t> FixIndices(const std::vector<int32_t>& indices) {
        auto fixed = std::vector<uint32_t>(indices.size());
        for (uint32_t i = 0; i < indices.size(); i++)
            if (indices[i] < 0)
                fixed[i] = (-indices[i]) - 1;
            else
                fixed[i] = indices[i];
        return fixed;
    }
}

namespace std {
    template <> struct hash<FbxLoader::vec3> {
        std::size_t operator()(const FbxLoader::vec3& v) const {
            std::size_t res = 0;
            FbxLoader::hash_combine(res, v.x);
            FbxLoader::hash_combine(res, v.y);
            FbxLoader::hash_combine(res, v.z);
            return res;
        }
    };

    template <> struct hash<FbxLoader::vec2> {
        std::size_t operator()(const FbxLoader::vec2& v) const {
            std::size_t res = 0;
            FbxLoader::hash_combine(res, v.x);
            FbxLoader::hash_combine(res, v.y);
            return res;
        }
    };

    template <> struct hash<FbxLoader::Vertex> {
        std::size_t operator()(const FbxLoader::Vertex& v) const {
            std::size_t result = 0;
            FbxLoader::hash_combine(result, v.pos);
            FbxLoader::hash_combine(result, v.norm);
            FbxLoader::hash_combine(result, v.binorm);
            FbxLoader::hash_combine(result, v.tang);
            FbxLoader::hash_combine(result, v.uv);
            return result;
        }
    };
}

#endif //FBXLOADER_MESH_H