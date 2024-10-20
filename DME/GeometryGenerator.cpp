#include "GeometryGenerator.h"
#include "ModelLoader.h"

using namespace std;
using namespace DirectX;

MeshData GeometryGenerator::CreateBox(const float scale) {
    MeshData meshData;
    meshData.vertices.resize(24);

    // �� ���� ������ ����
    float half = 0.5f * scale;

    // ��
    meshData.vertices[0] = Vertex(-half, -half, -half, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    meshData.vertices[1] = Vertex(-half, +half, -half, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    meshData.vertices[2] = Vertex(+half, +half, -half, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    meshData.vertices[3] = Vertex(+half, -half, -half, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    // ��
    meshData.vertices[4] = Vertex(+half, -half, +half, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    meshData.vertices[5] = Vertex(+half, +half, +half, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    meshData.vertices[6] = Vertex(-half, +half, +half, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    meshData.vertices[7] = Vertex(-half, -half, +half, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    // ��
    meshData.vertices[8] = Vertex(-half, +half, -half, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[9] = Vertex(-half, +half, +half, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[10] = Vertex(+half, +half, +half, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[11] = Vertex(+half, +half, -half, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

    // �Ʒ�
    meshData.vertices[12] = Vertex(-half, -half, +half, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[13] = Vertex(-half, -half, -half, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[14] = Vertex(+half, -half, -half, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[15] = Vertex(+half, -half, +half, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);

    // ����
    meshData.vertices[16] = Vertex(-half, -half, +half, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[17] = Vertex(-half, +half, +half, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[18] = Vertex(-half, +half, -half, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[19] = Vertex(-half, -half, -half, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // ������
    meshData.vertices[20] = Vertex(+half, -half, -half, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    meshData.vertices[21] = Vertex(+half, +half, -half, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    meshData.vertices[22] = Vertex(+half, +half, +half, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    meshData.vertices[23] = Vertex(+half, -half, +half, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    
    // �ε���
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

    // �� �ε�
    vector<MeshData> meshes = ModelLoader::Load(basePath, filename);

    // ���ؽ� ����ȭ ����

    // �� �࿡ ���� �ּҰ��� �ִ밪 ã��
    XMVECTOR vmin = XMVectorSet(1000.0f, 1000.0f, 1000.0f, 0.0f);
    XMVECTOR vmax = XMVectorSet(-1000.0f, -1000.0f, -1000.0f, 0.0f);
    for (auto& mesh : meshes) {
        for (auto& v : mesh.vertices) {
            XMVECTOR position = XMLoadFloat3(&v.position);
            vmin = XMVectorMin(vmin, position);
            vmax = XMVectorMax(vmax, position);
        }
    }

    // �޽��� �� �࿡�� �����ϴ� ���̿� �߽� ��ǥ ���
    XMVECTOR delta = XMVectorSubtract(vmax, vmin);
    XMVECTOR center = XMVectorScale(XMVectorAdd(vmax, vmin), 0.5f);

    // ���� �� �ִ밪�� �̿��Ͽ� ��� ���ؽ��� ���� ����ȭ ����
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
    // ���� �޽��� ��� �����͸� Ȱ���� ���� �޽��� �ܺθ� ����
    // ���� �޽��� ���ؽ��� �ݴ�� ����� ���� �� ����
    // ���콺 ��ġ�� �Ÿ� ������� ���� �޽��� Ȯ�� ������ ����

    std::vector<MeshData> shell;
    
    // �� �κ� �޽����� ���� ó��
    for (const auto& mesh : originalMeshData) {
        MeshData newMesh;
        for (const auto& v : mesh.vertices) {
            Vertex newV = v;

            // normal�� thickness��ŭ ���Ͽ� position�� ���ϱ�
            XMVECTOR positionVec = XMLoadFloat3(&v.position);
            XMVECTOR displacementVec = XMVectorScale(XMLoadFloat3(&v.normal), thickness);
            
            // ���ο� position ���� XMFLOAT3�� ��ȯ �� ����
            DirectX::XMStoreFloat3(&newV.position, positionVec + displacementVec);

            newMesh.vertices.push_back(newV);
        }

        newMesh.indices = mesh.indices;

        shell.push_back(newMesh);
    }

    return shell;
}