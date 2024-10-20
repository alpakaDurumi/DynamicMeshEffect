#include "ModelLoader.h"

#include <iostream>
#include <filesystem>

std::vector<MeshData> ModelLoader::Load(const std::string& basePath, const std::string& filename) {
    using namespace DirectX;

    std::vector<MeshData> meshes;

    // Importer 인스턴스 생성
    Assimp::Importer importer;

    // 플래그 설정
    // 1. 모든 primitive를 삼각형으로 설정
    // 2. D3D에 맞게 좌표계와 winding order 설정
    // 3. 노멀 벡터가 없는 경우 자동으로 생성
    // 4. 중복 정점 최적화
    const aiScene* pScene = importer.ReadFile(
        basePath + filename,
        aiProcess_Triangulate | aiProcess_SortByPType |
        aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_FlipWindingOrder |
        aiProcess_GenNormals|
        aiProcess_JoinIdenticalVertices);

    // 실패 시
    if (!pScene) {
        std::cout << "Failed to assimp ReadFile : " << basePath + filename << std::endl;
    }
    else {
        XMMATRIX tr = XMMatrixIdentity();   // 초기 변환 행렬(단위 행렬)
        ProcessNode(pScene->mRootNode, pScene, tr, meshes, basePath);
    }

    return meshes;
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, DirectX::XMMATRIX tr, std::vector<MeshData>& meshes, const std::string& basePath) {
    using namespace DirectX;

    // 해당 노드의 변환 행렬을 XMMATRIX에 로드
    XMMATRIX subTransform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&node->mTransformation)));
    // 누적되는 변환 계산
    subTransform = subTransform * tr;

    // 노멀 변환을 위한 invTranspose 계산
    XMMATRIX invTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, subTransform));

    // 노드에 속한 메쉬 처리
    for (UINT i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto newMesh = ProcessMesh(mesh, scene, basePath);
        // 각 버텍스에 대해 변환 적용
        for (auto& v : newMesh.vertices) {
            // 위치 변환
            XMVECTOR position = XMLoadFloat3(&v.position);
            position = XMVector3TransformCoord(position, subTransform);
            XMStoreFloat3(&v.position, position);

            // 노멀 변환
            // XMVector3TransformNormal은 자동적으로 마지막 행을 무시
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

    // 각 face에 대해 정점 복사
    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        Vertex v0, v1, v2;

        // face에 속한 세 정점에 대한 인덱스
        UINT i0 = face.mIndices[0];
        UINT i1 = face.mIndices[1];
        UINT i2 = face.mIndices[2];

        // face에 속한 세 정점
        v0.position = XMFLOAT3(mesh->mVertices[i0].x, mesh->mVertices[i0].y, mesh->mVertices[i0].z);
        v1.position = XMFLOAT3(mesh->mVertices[i1].x, mesh->mVertices[i1].y, mesh->mVertices[i1].z);
        v2.position = XMFLOAT3(mesh->mVertices[i2].x, mesh->mVertices[i2].y, mesh->mVertices[i2].z);

        // 노멀 벡터
        v0.normal = XMFLOAT3(mesh->mNormals[i0].x, mesh->mNormals[i0].y, mesh->mNormals[i0].z);
        v1.normal = XMFLOAT3(mesh->mNormals[i1].x, mesh->mNormals[i1].y, mesh->mNormals[i1].z);
        v2.normal = XMFLOAT3(mesh->mNormals[i2].x, mesh->mNormals[i2].y, mesh->mNormals[i2].z);
        XMStoreFloat3(&v0.normal, XMVector3Normalize(XMLoadFloat3(&v0.normal)));
        XMStoreFloat3(&v1.normal, XMVector3Normalize(XMLoadFloat3(&v1.normal)));
        XMStoreFloat3(&v2.normal, XMVector3Normalize(XMLoadFloat3(&v2.normal)));

        // 텍스처 좌표
        if (mesh->mTextureCoords[0]) {
            v0.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i0].x);
            v0.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i0].y);

            v1.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i1].x);
            v1.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i1].y);

            v2.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i2].x);
            v2.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i2].y);
        }

        // face normal 계산
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

    // 머티리얼 로드
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