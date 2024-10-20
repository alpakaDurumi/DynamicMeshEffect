#pragma once

#include <vector>
#include <string>

#include <windows.h>

#include "Vertex.h"

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
    std::string textureFilename;
};

struct ShellMeshData {
    std::vector<ShellVertex> vertices;
    std::vector<UINT> indices;
    std::string textureFilename;
};

class GeometryGenerator {
public:
    static MeshData CreateBox(const float scale = 1.0f);
    static MeshData CreateSphere(const float radius, const int numSlices, const int numStacks);
    static std::vector<MeshData> ReadFromFile(std::string basePath, std::string filename);
    static std::vector<ShellMeshData> CreateShell(const std::vector<MeshData>& originalMeshData, float thickness);
};