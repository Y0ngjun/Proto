#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "VertexArray.h"
#include "Buffer.h"

namespace Proto
{
    // 간단한 메시 데이터 구조
    struct MeshData
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<uint32_t> indices;
    };

    class MeshLoader
    {
    public:
        // 파일에서 메시 로드
        static std::shared_ptr<VertexArray> LoadMesh(const std::string& filePath);

        // Primitive 메시 생성 (코드)
        static std::shared_ptr<VertexArray> CreateCube();
        static std::shared_ptr<VertexArray> CreatePlane();

    private:
        MeshLoader() = default;

        // OBJ 파일 파싱 (향후 확장용)
        static MeshData ParseOBJ(const std::string& filePath);
    };
}
