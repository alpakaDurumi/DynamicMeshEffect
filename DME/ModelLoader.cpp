#include "ModelLoader.h"

#include <iostream>
#include <filesystem>

std::vector<MeshData> ModelLoader::Load(const std::string& basePath, const std::string& filename) {
    using namespace DirectX;

    std::vector<MeshData> meshes;

    // Importer �ν��Ͻ� ����
    Assimp::Importer importer;

    // �÷��� ����
    // 1. ��� primitive�� �ﰢ������ ����
    // 2. D3D�� �°� ��ǥ��� winding order ����
    // 3. ��� ���Ͱ� ���� ��� �ڵ����� ����
    const aiScene* pScene = importer.ReadFile(
        basePath + filename,
        aiProcess_Triangulate | aiProcess_SortByPType |
        aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_FlipWindingOrder |
        aiProcess_GenNormals);

    // ���� ��
    if (!pScene) {
        std::cout << "Failed to assimp ReadFile : " << basePath + filename << std::endl;
    }
    else {
        XMMATRIX tr = XMMatrixIdentity();   // �ʱ� ��ȯ ���(���� ���)
        ProcessNode(pScene->mRootNode, pScene, tr, meshes, basePath);
    }

    return meshes;
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, DirectX::XMMATRIX tr, std::vector<MeshData>& meshes, const std::string& basePath) {
    using namespace DirectX;

    // �ش� ����� ��ȯ ����� XMMATRIX�� �ε�
    XMMATRIX subTransform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation)));
    // �����Ǵ� ��ȯ ���
    subTransform = subTransform * tr;

    // ��� ��ȯ�� ���� invTranspose ���
    XMMATRIX invTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, subTransform));

    // ��忡 ���� �޽� ó��
    for (UINT i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto newMesh = ProcessMesh(mesh, scene, basePath);
        // �� ���ؽ��� ���� ��ȯ ����
        for (auto& v : newMesh.vertices) {
            // ��ġ ��ȯ
            XMVECTOR position = XMLoadFloat3(&v.position);
            position = XMVector3TransformCoord(position, subTransform);
            XMStoreFloat3(&v.position, position);

            // ��� ��ȯ
            // XMVector3TransformNormal�� �ڵ������� ������ ���� ����
            XMVECTOR normal = XMLoadFloat3(&v.normal);
            normal = XMVector3TransformNormal(normal, invTranspose);
            XMStoreFloat3(&v.normal, normal);
        }

        meshes.push_back(newMesh);
    }

    for (UINT i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, subTransform, meshes, basePath);
    }
}

MeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& basePath) {
    using namespace DirectX;

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    // �޽��� �� ���ؽ��� �� �ε�
    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        // ��� ���� ����ȭ
        XMVECTOR normalVec = XMVector3Normalize(XMLoadFloat3(&vertex.normal));
        XMStoreFloat3(&vertex.normal, normalVec);

        // �ؽ�ó ��ǥ �ε�
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }

        vertices.push_back(vertex);
    }

    // �ε��� ���� �ε�
    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    MeshData newMesh;
    newMesh.vertices = vertices;
    newMesh.indices = indices;

    // ��Ƽ���� �ε�
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            // �ؽ�ó ������ �� ���ϰ� ���� ��ġ�� �ִٰ� ����
            aiString filepath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);

            std::string fullPath = basePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());
            newMesh.textureFilename = fullPath;
        }
    }

    return newMesh;
}