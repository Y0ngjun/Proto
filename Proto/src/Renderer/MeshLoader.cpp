#include "MeshLoader.h"
#include "Buffer.h"
#include <glm/gtc/constants.hpp>

namespace Proto
{
	static std::shared_ptr<VertexArray> CreateVAO(const float* vertices, uint32_t vertexSize, const uint32_t* indices, uint32_t indexCount)
	{
		auto vbo = std::make_shared<VertexBuffer>(vertices, vertexSize);
		auto ebo = std::make_shared<IndexBuffer>(indices, indexCount);
		auto vao = std::make_shared<VertexArray>();

		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ebo);
		vao->Unbind();

		return vao;
	}

	std::shared_ptr<VertexArray> MeshLoader::LoadMesh(const std::string& filePath)
	{
		if (filePath.find("cube") != std::string::npos)
			return CreateCube();
		
		if (filePath.find("plane") != std::string::npos)
			return CreatePlane();

		return nullptr;
	}

	std::shared_ptr<VertexArray> MeshLoader::CreateCube()
	{
		float vertices[] = {
			// Front face (Z = 0.5)
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			// Back face (Z = -0.5)
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			// Left face (X = -0.5)
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			// Right face (X = 0.5)
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			// Top face (Y = 0.5)
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			// Bottom face (Y = -0.5)
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f
		};

		uint32_t indices[] = {
			 0,  1,  2,  2,  3,  0, // Front
			 4,  5,  6,  6,  7,  4, // Back
			 8,  9, 10, 10, 11,  8, // Left
			12, 13, 14, 14, 15, 12, // Right
			16, 17, 18, 18, 19, 16, // Top
			20, 21, 22, 22, 23, 20  // Bottom
		};

		return CreateVAO(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(uint32_t));
	}

	std::shared_ptr<VertexArray> MeshLoader::CreatePlane()
	{
		float vertices[] = {
			-2.5f, 0.0f,  2.5f,  0.0f, 1.0f, 0.0f,
			 2.5f, 0.0f,  2.5f,  0.0f, 1.0f, 0.0f,
			 2.5f, 0.0f, -2.5f,  0.0f, 1.0f, 0.0f,
			-2.5f, 0.0f, -2.5f,  0.0f, 1.0f, 0.0f
		};

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		return CreateVAO(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(uint32_t));
	}

	std::shared_ptr<VertexArray> MeshLoader::CreateSphere(float radius, uint32_t sectorCount, uint32_t stackCount)
	{
		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		const float lengthInv = 1.0f / radius;
		const float sectorStep = 2 * glm::pi<float>() / sectorCount;
		const float stackStep = glm::pi<float>() / stackCount;

		for (uint32_t i = 0; i <= stackCount; ++i)
		{
			const float stackAngle = glm::pi<float>() / 2 - i * stackStep;
			const float xy = radius * cosf(stackAngle);
			const float z = radius * sinf(stackAngle);

			for (uint32_t j = 0; j <= sectorCount; ++j)
			{
				const float sectorAngle = j * sectorStep;
				const float x = xy * cosf(sectorAngle);
				const float y = xy * sinf(sectorAngle);

				// Position
				vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
				// Normal
				vertices.push_back(x * lengthInv); vertices.push_back(y * lengthInv); vertices.push_back(z * lengthInv);
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

		return CreateVAO(vertices.data(), (uint32_t)(vertices.size() * sizeof(float)), indices.data(), (uint32_t)indices.size());
	}

	std::shared_ptr<VertexArray> MeshLoader::CreateCylinder(float radius, float height, uint32_t sectorCount)
	{
		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		const float sectorStep = 2 * glm::pi<float>() / sectorCount;
		const float halfHeight = height / 2.0f;

		// Side
		for (uint32_t i = 0; i <= sectorCount; ++i)
		{
			const float sectorAngle = i * sectorStep;
			const float x = radius * cosf(sectorAngle);
			const float z = radius * sinf(sectorAngle);
			const float nx = cosf(sectorAngle);
			const float nz = sinf(sectorAngle);

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

		// Top Cap
		const uint32_t topCenterIndex = (uint32_t)(vertices.size() / 6);
		vertices.push_back(0.0f); vertices.push_back(halfHeight); vertices.push_back(0.0f);
		vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

		const uint32_t topEdgeOffset = (uint32_t)(vertices.size() / 6);
		for (uint32_t i = 0; i <= sectorCount; ++i)
		{
			const float sectorAngle = i * sectorStep;
			const float x = radius * cosf(sectorAngle);
			const float z = radius * sinf(sectorAngle);
			vertices.push_back(x); vertices.push_back(halfHeight); vertices.push_back(z);
			vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
		}

		for (uint32_t i = 0; i < sectorCount; ++i)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(topEdgeOffset + i);
			indices.push_back(topEdgeOffset + i + 1);
		}

		// Bottom Cap
		const uint32_t bottomCenterIndex = (uint32_t)(vertices.size() / 6);
		vertices.push_back(0.0f); vertices.push_back(-halfHeight); vertices.push_back(0.0f);
		vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);

		const uint32_t bottomEdgeOffset = (uint32_t)(vertices.size() / 6);
		for (uint32_t i = 0; i <= sectorCount; ++i)
		{
			const float sectorAngle = i * sectorStep;
			const float x = radius * cosf(sectorAngle);
			const float z = radius * sinf(sectorAngle);
			vertices.push_back(x); vertices.push_back(-halfHeight); vertices.push_back(z);
			vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);
		}

		for (uint32_t i = 0; i < sectorCount; ++i)
		{
			indices.push_back(bottomCenterIndex);
			indices.push_back(bottomEdgeOffset + i + 1);
			indices.push_back(bottomEdgeOffset + i);
		}

		return CreateVAO(vertices.data(), (uint32_t)(vertices.size() * sizeof(float)), indices.data(), (uint32_t)indices.size());
	}
}
