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
    // 4. �ߺ� ���� ����ȭ
    const aiScene* pScene = importer.ReadFile(
        basePath + filename,
        aiProcess_Triangulate | aiProcess_SortByPType |
        aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_FlipWindingOrder |
        aiProcess_GenNormals|
        aiProcess_JoinIdenticalVertices);

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

    // �� face�� ���� ���� ����
    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        Vertex v0, v1, v2;

        // face�� ���� �� ������ ���� �ε���
        UINT i0 = face.mIndices[0];
        UINT i1 = face.mIndices[1];
        UINT i2 = face.mIndices[2];

        // face�� ���� �� ����
        v0.position = XMFLOAT3(mesh->mVertices[i0].x, mesh->mVertices[i0].y, mesh->mVertices[i0].z);
        v1.position = XMFLOAT3(mesh->mVertices[i1].x, mesh->mVertices[i1].y, mesh->mVertices[i1].z);
        v2.position = XMFLOAT3(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);

        // ��� ����
        v0.normal = XMFLOAT3(mesh->mNormals[i0].x, mesh->mNormals[i0].y, mesh->mNormals[i0].z);
        v1.normal = XMFLOAT3(mesh->mNormals[i1].x, mesh->mNormals[i1].y, mesh->mNormals[i1].z);
        v2.normal = XMFLOAT3(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
        XMStoreFloat3(&v0.normal, XMVector3Normalize(XMLoadFloat3(&v0.normal)));
        XMStoreFloat3(&v1.normal, XMVector3Normalize(XMLoadFloat3(&v1.normal)));
        XMStoreFloat3(&v2.normal, XMVector3Normalize(XMLoadFloat3(&v2.normal)));

        // �ؽ�ó ��ǥ
        if (mesh->mTextureCoords[0]) {
            v0.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i0].x);
            v0.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i0].y);

            v1.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i1].x);
            v1.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i1].y);

            v2.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i2].x);
            v2.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i2].y);
        }

        // face normal ���
        XMVECTOR faceNormal = XMVector3Normalize(XMVector3Cross(
            XMLoadFloat3(&v1.position) - XMLoadFloat3(&v0.position),
            XMLoadFloat3(&v2.position) - XMLoadFloat3(&v0.position)));
        XMStoreFloat3(&v0.faceNormal, faceNormal);
        XMStoreFloat3(&v1.faceNormal, faceNormal);
        XMStoreFloat3(&v2.faceNormal, faceNormal);

        vertices.push_back(v0);
        indices.push_back(static_cast<UINT>(vertices.size() - 1));
        vertices.push_back(v1);
        indices.push_back(static_cast<UINT>(vertices.size() - 1));
        vertices.push_back(v2);
        indices.push_back(static_cast<UINT>(vertices.size() - 1));
    }

    MeshData newMesh;
    newMesh.vertices = vertices;
    newMesh.indices = indices;

    // ��Ƽ���� �ε�
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString filepath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);

            std::string fullPath = basePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());
            newMesh.textureFilename = fullPath;
        }
    }

    return newMesh;
}