#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "GeometryGenerator.h"

class ModelLoader {
public:
    static std::vector<MeshData> Load(const std::string& basePath, const std::string& filename);
    static void ProcessNode(aiNode* node, const aiScene* scene, DirectX::XMMATRIX tr, std::vector<MeshData>& meshes, const std::string& basePath);
    static MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& basePath);
};