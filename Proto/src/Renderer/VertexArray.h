/*
 * OpenGL 정점 배열 객체(Vertex Array Object)를 추상화한 클래스입니다.
 * 정점 버퍼와 인덱스 버퍼를 하나로 묶어 관리하며, 에셋 형태로 취급됩니다.
 */

#pragma once

#include <memory>
#include <vector>

#include "../Asset/Asset.h"

namespace Proto
{
	class VertexBuffer;
	class IndexBuffer;

	class VertexArray : public Asset
	{
	public:
		virtual AssetType GetType() const override
		{
			return AssetType::Mesh;
		}

		VertexArray();
		virtual ~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const
		{
			return m_VertexBuffers;
		}

		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const
		{
			return m_IndexBuffer;
		}

	private:
		uint32_t m_RendererID = 0;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}