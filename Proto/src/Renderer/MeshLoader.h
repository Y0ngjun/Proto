/*
 * 3D 모델(메시) 데이터를 로드하고 기본 기하학적 도형을 생성하는 팩토리 클래스입니다.
 * 정점(Vertex)과 인덱스(Index) 데이터를 기반으로 VertexArray 객체를 반환합니다.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Proto
{
	class VertexArray;

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
		~MeshLoader() = default;
	};
}
