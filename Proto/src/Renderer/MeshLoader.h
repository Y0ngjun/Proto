#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "VertexArray.h"
#include "Buffer.h"

namespace Proto
{
    class MeshLoader
    {
    public:
        static std::shared_ptr<VertexArray> LoadMesh(const std::string& filePath);

        static std::shared_ptr<VertexArray> CreateCube();
        static std::shared_ptr<VertexArray> CreatePlane();
        static std::shared_ptr<VertexArray> CreateSphere(float radius = 0.5f, uint32_t sectorCount = 36, uint32_t stackCount = 18);
        static std::shared_ptr<VertexArray> CreateCylinder(float radius = 0.5f, float height = 1.0f, uint32_t sectorCount = 36);

    private:
        MeshLoader() = default;
    };
}
