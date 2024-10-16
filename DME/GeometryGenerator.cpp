#include "GeometryGenerator.h"

using namespace std;

MeshData GeometryGenerator::CreateBox(const float scale = 1.0f) {
    MeshData meshData;

    // �� ���� ������ ����
    float half = 0.5f * scale;

    // ��
    meshData.vertices[0] = Vertex(-half, -half, -half, 0.0f, 0.0f, -1.0f);
    meshData.vertices[1] = Vertex(-half, +half, -half, 0.0f, 0.0f, -1.0f);
    meshData.vertices[2] = Vertex(+half, +half, -half, 0.0f, 0.0f, -1.0f);
    meshData.vertices[3] = Vertex(+half, -half, -half, 0.0f, 0.0f, -1.0f);

    // ��
    meshData.vertices[4] = Vertex(+half, -half, +half, 0.0f, 0.0f, 1.0f);
    meshData.vertices[5] = Vertex(+half, +half, +half, 0.0f, 0.0f, 1.0f);
    meshData.vertices[6] = Vertex(-half, +half, +half, 0.0f, 0.0f, 1.0f);
    meshData.vertices[7] = Vertex(-half, -half, +half, 0.0f, 0.0f, 1.0f);

    // ��
    meshData.vertices[8] = Vertex(-half, +half, -half, 0.0f, 1.0f, 0.0f);
    meshData.vertices[9] = Vertex(-half, +half, +half, 0.0f, 1.0f, 0.0f);
    meshData.vertices[10] = Vertex(+half, +half, +half, 0.0f, 1.0f, 0.0f);
    meshData.vertices[11] = Vertex(+half, +half, -half, 0.0f, 1.0f, 0.0f);

    // �Ʒ�
    meshData.vertices[12] = Vertex(-half, -half, +half, 0.0f, -1.0f, 0.0f);
    meshData.vertices[13] = Vertex(-half, -half, -half, 0.0f, -1.0f, 0.0f);
    meshData.vertices[14] = Vertex(+half, -half, -half, 0.0f, -1.0f, 0.0f);
    meshData.vertices[15] = Vertex(+half, -half, +half, 0.0f, -1.0f, 0.0f);

    // ����
    meshData.vertices[16] = Vertex(-half, -half, +half, -1.0f, 0.0f, 0.0f);
    meshData.vertices[17] = Vertex(-half, +half, +half, -1.0f, 0.0f, 0.0f);
    meshData.vertices[18] = Vertex(-half, +half, -half, -1.0f, 0.0f, 0.0f);
    meshData.vertices[19] = Vertex(-half, -half, -half, -1.0f, 0.0f, 0.0f);

    // ������
    meshData.vertices[20] = Vertex(+half, -half, -half, 1.0f, 0.0f, 0.0f);
    meshData.vertices[21] = Vertex(+half, +half, -half, 1.0f, 0.0f, 0.0f);
    meshData.vertices[22] = Vertex(+half, +half, +half, 1.0f, 0.0f, 0.0f);
    meshData.vertices[23] = Vertex(+half, -half, +half, 1.0f, 0.0f, 0.0f);
    
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