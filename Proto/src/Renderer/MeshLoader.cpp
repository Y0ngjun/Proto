#include "MeshLoader.h"
#include "Buffer.h"

namespace Proto
{
    std::shared_ptr<VertexArray> MeshLoader::LoadMesh(const std::string& filePath)
    {
        // 현재는 Primitive 메시만 지원
        // 향후 OBJ 등의 파일 형식 지원 가능
        if (filePath.find("cube") != std::string::npos)
        {
            return CreateCube();
        }
        else if (filePath.find("plane") != std::string::npos)
        {
            return CreatePlane();
        }

        return nullptr;
    }

    std::shared_ptr<VertexArray> MeshLoader::CreateCube()
    {
        float vertices[] = {
            // Front face (Z = 0.5)
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 0
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 1
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 2
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 3
            // Back face (Z = -0.5)
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 4
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 5
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 6
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 7
            // Left face (X = -0.5)
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 8
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 9
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 10
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 11
            // Right face (X = 0.5)
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 12
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 13
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 14
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 15
            // Top face (Y = 0.5)
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // 16
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // 17
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 18
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 19
            // Bottom face (Y = -0.5)
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 20
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 21
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // 22
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f  // 23
        };

        uint32_t indices[] = {
             0,  1,  2,  2,  3,  0, // Front
             4,  5,  6,  6,  7,  4, // Back
             8,  9, 10, 10, 11,  8, // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom
        };

        auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
        auto ebo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
        auto vao = std::make_shared<VertexArray>();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ebo);
        vao->Unbind();

        return vao;
    }

    std::shared_ptr<VertexArray> MeshLoader::CreatePlane()
    {
        float planeVertices[] = {
            // Position               // Normal
            -2.5f, 0.0f,  2.5f,       0.0f, 1.0f, 0.0f, // 0: Bottom-left
             2.5f, 0.0f,  2.5f,       0.0f, 1.0f, 0.0f, // 1: Bottom-right
             2.5f, 0.0f, -2.5f,       0.0f, 1.0f, 0.0f, // 2: Top-right
            -2.5f, 0.0f, -2.5f,       0.0f, 1.0f, 0.0f  // 3: Top-left
        };

        uint32_t planeIndices[] = { 0, 1, 2, 2, 3, 0 };

        auto planeVBO = std::make_shared<VertexBuffer>(planeVertices, sizeof(planeVertices));
        auto planeEBO = std::make_shared<IndexBuffer>(planeIndices, sizeof(planeIndices) / sizeof(uint32_t));
        auto planeVAO = std::make_shared<VertexArray>();
        planeVAO->AddVertexBuffer(planeVBO);
        planeVAO->SetIndexBuffer(planeEBO);
        planeVAO->Unbind();

        return planeVAO;
    }

    MeshData MeshLoader::ParseOBJ(const std::string& filePath)
    {
        // 향후 구현
        return MeshData();
    }
}
