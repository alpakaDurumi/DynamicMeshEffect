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
    const aiScene* pScene = importer.ReadFile(
        basePath + filename,
        aiProcess_Triangulate | aiProcess_SortByPType |
        aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_FlipWindingOrder |
        aiProcess_GenNormals);

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

    // 메쉬의 각 버텍스의 값 로드
    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        // 노멀 벡터 정규화
        XMVECTOR normalVec = XMVector3Normalize(XMLoadFloat3(&vertex.normal));
        XMStoreFloat3(&vertex.normal, normalVec);

        // 텍스처 좌표 로드
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
            vertex.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
        }

        vertices.push_back(vertex);
    }

    // 인덱스 버퍼 로드
    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    MeshData newMesh;
    newMesh.vertices = vertices;
    newMesh.indices = indices;

    // 머티리얼 로드
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            // 텍스처 파일은 모델 파일과 같은 위치에 있다고 가정
            aiString filepath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);

            std::string fullPath = basePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());
            newMesh.textureFilename = fullPath;
        }
    }

    return newMesh;
}