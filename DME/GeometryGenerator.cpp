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

MeshData GeometryGenerator::CreateSphere(const float radius, const int numSlices, const int numStacks) {
    const float dTheta = -XM_2PI / numSlices;
    const float dPhi = -XM_PI / numStacks;

    MeshData meshData;

    vector<Vertex>& vertices = meshData.vertices;

    for (int j = 0; j <= numStacks; j++) {
        XMFLOAT3 temp(0.0f, -radius, 0.0f);
        XMVECTOR stackStartPoint = XMVector3Transform(XMLoadFloat3(&temp), XMMatrixRotationZ(dPhi * j));
        for (int i = 0; i <= numSlices; i++) {
            Vertex v;

            XMVECTOR position = XMVector3Transform(stackStartPoint, XMMatrixRotationY(dTheta * static_cast<float>(i)));
            XMStoreFloat3(&v.position, position);
            XMStoreFloat3(&v.normal, XMVector3Normalize(position));
            v.texCoord = XMFLOAT2(static_cast<float>(i) / numSlices, 1.0f - static_cast<float>(j) / numStacks);

            vertices.push_back(v);
        }
    }

    vector<UINT>& indices = meshData.indices;

    for (int j = 0; j < numStacks; j++) {
        const int offset = (numSlices + 1) * j;
        for (int i = 0; i < numSlices; i++) {
            indices.push_back(offset + i);
            indices.push_back(offset + i + numSlices + 1);
            indices.push_back(offset + i + 1 + numSlices + 1);

            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + numSlices + 1);
            indices.push_back(offset + i + 1);
        }
    }

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

std::vector<ShellMeshData> GeometryGenerator::CreateShell(const std::vector<MeshData>& originalMeshData, float thickness) {
    // ���� �޽��� ��� �����͸� Ȱ���� ���� �޽��� �ܺθ� ����
    // ���� �޽��� ���ؽ��� �ݴ�� ����� ���� �� ����
    // ���콺 ��ġ�� �Ÿ� ������� ���� �޽��� Ȯ�� ������ ����

    std::vector<ShellMeshData> shell;
    
    // �� �κ� �޽����� ���� ó��
    for (const auto& mesh : originalMeshData) {
        ShellMeshData newMesh;
        newMesh.vertices.reserve(mesh.vertices.size() * 2);

        // �ٱ��� ���ؽ�
        for (const auto& v : mesh.vertices) {
            ShellVertex newV;
            newV.position = v.position;
            newV.normal = v.normal;
            newV.texCoord = v.texCoord;

            // normal�� thickness��ŭ ���Ͽ� position�� ���ϱ�
            XMVECTOR positionVec = XMLoadFloat3(&v.position);
            XMVECTOR displacementVec = XMVectorScale(XMLoadFloat3(&v.normal), thickness);
            DirectX::XMStoreFloat3(&newV.position, positionVec + displacementVec);

            // �ٱ������� ǥ��
            newV.isOutside = 1;
            newMesh.vertices.push_back(newV);
        }

        // ���� ���ؽ�
        for (const auto& v : mesh.vertices) {
            ShellVertex newV;
            newV.position = v.position;
            newV.normal = v.normal;
            newV.texCoord = v.texCoord;

            // �������� ǥ��
            newV.isOutside = 0;
            newMesh.vertices.push_back(newV);
        }

        // �ε��� �߰�
        const std::vector<UINT>& indices = mesh.indices;
        const size_t indexCount = indices.size();
        for (size_t i = 0; i < indexCount; i += 3) {
            // 3���� �ε����� ������
            UINT idx0 = indices[i];
            UINT idx1 = indices[i + 1];
            UINT idx2 = indices[i + 2];

            // �ٱ��� ���ؽ� �ε���
            UINT outerIdx0 = idx0;
            UINT outerIdx1 = idx1;
            UINT outerIdx2 = idx2;

            // ���� ���ؽ� �ε���
            UINT innerIdx0 = idx0 + mesh.vertices.size();
            UINT innerIdx1 = idx1 + mesh.vertices.size();
            UINT innerIdx2 = idx2 + mesh.vertices.size();

            // �ε��� �߰�: �ٱ��� �ﰢ��
            newMesh.indices.push_back(outerIdx0);
            newMesh.indices.push_back(outerIdx1);
            newMesh.indices.push_back(outerIdx2);

            // �ε��� �߰�: ���� �ﰢ��
            newMesh.indices.push_back(innerIdx2);
            newMesh.indices.push_back(innerIdx1);
            newMesh.indices.push_back(innerIdx0);

            // ���� �ﰢ�� �ε��� �߰�

            // 0 1
            newMesh.indices.push_back(innerIdx1);
            newMesh.indices.push_back(outerIdx1);
            newMesh.indices.push_back(outerIdx0);

            newMesh.indices.push_back(innerIdx1);
            newMesh.indices.push_back(outerIdx0);
            newMesh.indices.push_back(innerIdx0);

            // 1 2
            newMesh.indices.push_back(innerIdx2);
            newMesh.indices.push_back(outerIdx2);
            newMesh.indices.push_back(outerIdx1);

            newMesh.indices.push_back(innerIdx2);
            newMesh.indices.push_back(outerIdx1);
            newMesh.indices.push_back(innerIdx1);

            // 0 2
            newMesh.indices.push_back(innerIdx0);
            newMesh.indices.push_back(outerIdx0);
            newMesh.indices.push_back(outerIdx2);

            newMesh.indices.push_back(innerIdx0);
            newMesh.indices.push_back(outerIdx2);
            newMesh.indices.push_back(innerIdx2);
        }

        shell.push_back(newMesh);
    }

    return shell;
}