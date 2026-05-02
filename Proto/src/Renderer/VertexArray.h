#pragma once

#include <memory>
#include <vector>

#include "Buffer.h"
#include "../Asset/Asset.h"

namespace Proto
{
	class VertexArray : public Asset
	{
	public:
		virtual AssetType GetType() const override { return AssetType::Mesh; }

		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}