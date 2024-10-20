#include "GeometryGenerator.h"
#include "ModelLoader.h"

using namespace std;
using namespace DirectX;

MeshData GeometryGenerator::CreateBox(const float scale) {
    MeshData meshData;
    meshData.vertices.resize(24);

    // 한 변의 길이의 절반
    float half = 0.5f * scale;

    // 앞
    meshData.vertices[0] = Vertex(-half, -half, -half, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    meshData.vertices[1] = Vertex(-half, +half, -half, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    meshData.vertices[2] = Vertex(+half, +half, -half, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    meshData.vertices[3] = Vertex(+half, -half, -half, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    // 뒤
    meshData.vertices[4] = Vertex(+half, -half, +half, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    meshData.vertices[5] = Vertex(+half, +half, +half, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    meshData.vertices[6] = Vertex(-half, +half, +half, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    meshData.vertices[7] = Vertex(-half, -half, +half, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    // 위
    meshData.vertices[8] = Vertex(-half, +half, -half, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[9] = Vertex(-half, +half, +half, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[10] = Vertex(+half, +half, +half, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[11] = Vertex(+half, +half, -half, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

    // 아래
    meshData.vertices[12] = Vertex(-half, -half, +half, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[13] = Vertex(-half, -half, -half, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[14] = Vertex(+half, -half, -half, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[15] = Vertex(+half, -half, +half, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);

    // 왼쪽
    meshData.vertices[16] = Vertex(-half, -half, +half, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[17] = Vertex(-half, +half, +half, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[18] = Vertex(-half, +half, -half, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[19] = Vertex(-half, -half, -half, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // 오른쪽
    meshData.vertices[20] = Vertex(+half, -half, -half, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[21] = Vertex(+half, +half, -half, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[22] = Vertex(+half, +half, +half, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[23] = Vertex(+half, -half, +half, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    
    // 인덱스
    meshData.indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    return meshData;
}

vector<MeshData> GeometryGenerator::ReadFromFile(std::string basePath, std::string filename) {
    using namespace DirectX;

    // 모델 로드
    vector<MeshData> meshes = ModelLoader::Load(basePath, filename);

    // 버텍스 정규화 과정

    // 각 축에 대한 최소값과 최대값 찾기
    XMVECTOR vmin = XMVectorSet(1000.0f, 1000.0f, 1000.0f, 0.0f);
    XMVECTOR vmax = XMVectorSet(-1000.0f, -1000.0f, -1000.0f, 0.0f);
    for (auto& mesh : meshes) {
        for (auto& v : mesh.vertices) {
            XMVECTOR position = XMLoadFloat3(&v.position);
            vmin = XMVectorMin(vmin, position);
            vmax = XMVectorMax(vmax, position);
        }
    }

    // 메쉬가 각 축에서 차지하는 길이와 중심 좌표 계산
    XMVECTOR delta = XMVectorSubtract(vmax, vmin);
    XMVECTOR center = XMVectorScale(XMVectorAdd(vmax, vmin), 0.5f);

    // 길이 중 최대값을 이용하여 모든 버텍스에 대해 정규화 진행
    float dl = XMMax(XMMax(XMVectorGetX(delta), XMVectorGetY(delta)), XMVectorGetZ(delta));
    for (auto& mesh : meshes) {
        for (auto& v : mesh.vertices) {
            XMVECTOR position = XMLoadFloat3(&v.position);
            position = XMVectorDivide(XMVectorSubtract(position, center), XMVectorReplicate(dl));
            XMStoreFloat3(&v.position, position);
        }
    }

    return meshes;
}

std::vector<MeshData> GeometryGenerator::CreateShell(const std::vector<MeshData>& originalMeshData, float thickness) {
    // 원본 메쉬의 노멀 데이터를 활용해 껍질 메쉬의 외부를 생성
    // 원본 메쉬의 버텍스를 반대로 뒤집어서 내부 면 생성
    // 마우스 위치와 거리 기반으로 껍질 메쉬의 확장 범위를 조절

    std::vector<MeshData> shell;
    
    // 각 부분 메쉬마다 따로 처리
    for (const auto& mesh : originalMeshData) {
        MeshData newMesh;
        for (const auto& v : mesh.vertices) {
            Vertex newV = v;

            // normal을 thickness만큼 곱하여 position에 더하기
            XMVECTOR positionVec = XMLoadFloat3(&v.position);
            XMVECTOR displacementVec = XMVectorScale(XMLoadFloat3(&v.normal), thickness);
            
            // 새로운 position 값을 XMFLOAT3로 변환 후 저장
            DirectX::XMStoreFloat3(&newV.position, positionVec + displacementVec);

            newMesh.vertices.push_back(newV);
        }

        newMesh.indices = mesh.indices;

        shell.push_back(newMesh);
    }

    return shell;
}