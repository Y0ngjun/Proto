#include "MeshLoader.h"
#include "Buffer.h"
#include <glm/gtc/constants.hpp>

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

    std::shared_ptr<VertexArray> MeshLoader::CreateSphere(float radius, uint32_t sectorCount, uint32_t stackCount)
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        float lengthInv = 1.0f / radius;
        float sectorStep = 2 * glm::pi<float>() / sectorCount;
        float stackStep = glm::pi<float>() / stackCount;
        float sectorAngle, stackAngle;

        for (uint32_t i = 0; i <= stackCount; ++i)
        {
            stackAngle = glm::pi<float>() / 2 - i * stackStep;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (uint32_t j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;

                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);

                // Position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normal
                vertices.push_back(x * lengthInv);
                vertices.push_back(y * lengthInv);
                vertices.push_back(z * lengthInv);
            }
        }

        for (uint32_t i = 0; i < stackCount; ++i)
        {
            uint32_t k1 = i * (sectorCount + 1);
            uint32_t k2 = k1 + sectorCount + 1;

            for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        auto vbo = std::make_shared<VertexBuffer>(vertices.data(), (uint32_t)(vertices.size() * sizeof(float)));
        auto ebo = std::make_shared<IndexBuffer>(indices.data(), (uint32_t)indices.size());
        auto vao = std::make_shared<VertexArray>();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ebo);
        vao->Unbind();

        return vao;
    }

    std::shared_ptr<VertexArray> MeshLoader::CreateCylinder(float radius, float height, uint32_t sectorCount)
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        float sectorStep = 2 * glm::pi<float>() / sectorCount;
        float halfHeight = height / 2.0f;

        // 1. 측면 (Side)
        for (uint32_t i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            float nx = cosf(sectorAngle);
            float nz = sinf(sectorAngle);

            // Top
            vertices.push_back(x); vertices.push_back(halfHeight); vertices.push_back(z);
            vertices.push_back(nx); vertices.push_back(0.0f); vertices.push_back(nz);

            // Bottom
            vertices.push_back(x); vertices.push_back(-halfHeight); vertices.push_back(z);
            vertices.push_back(nx); vertices.push_back(0.0f); vertices.push_back(nz);
        }

        for (uint32_t i = 0; i < sectorCount; ++i)
        {
            uint32_t k1 = i * 2;
            uint32_t k2 = k1 + 1;
            uint32_t k3 = (i + 1) * 2;
            uint32_t k4 = k3 + 1;

            indices.push_back(k1); indices.push_back(k2); indices.push_back(k3);
            indices.push_back(k3); indices.push_back(k2); indices.push_back(k4);
        }

        // 2. 윗면 (Top Cap)
        uint32_t topCenterIndex = (uint32_t)(vertices.size() / 6);
        vertices.push_back(0.0f); vertices.push_back(halfHeight); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

        uint32_t topEdgeOffset = (uint32_t)(vertices.size() / 6);
        for (uint32_t i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            vertices.push_back(x); vertices.push_back(halfHeight); vertices.push_back(z);
            vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        }

        for (uint32_t i = 0; i < sectorCount; ++i)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(topEdgeOffset + i);
            indices.push_back(topEdgeOffset + i + 1);
        }

        // 3. 아랫면 (Bottom Cap)
        uint32_t bottomCenterIndex = (uint32_t)(vertices.size() / 6);
        vertices.push_back(0.0f); vertices.push_back(-halfHeight); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);

        uint32_t bottomEdgeOffset = (uint32_t)(vertices.size() / 6);
        for (uint32_t i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            vertices.push_back(x); vertices.push_back(-halfHeight); vertices.push_back(z);
            vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);
        }

        for (uint32_t i = 0; i < sectorCount; ++i)
        {
            indices.push_back(bottomCenterIndex);
            indices.push_back(bottomEdgeOffset + i + 1);
            indices.push_back(bottomEdgeOffset + i);
        }

        auto vbo = std::make_shared<VertexBuffer>(vertices.data(), (uint32_t)(vertices.size() * sizeof(float)));
        auto ebo = std::make_shared<IndexBuffer>(indices.data(), (uint32_t)indices.size());
        auto vao = std::make_shared<VertexArray>();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ebo);
        vao->Unbind();

        return vao;
    }

    MeshData MeshLoader::ParseOBJ(const std::string& filePath)
    {
        // 향후 구현
        return MeshData();
    }
}
