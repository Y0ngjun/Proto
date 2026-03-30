#pragma once

#include <cstdint>

namespace Proto {

	class VertexBuffer {
	public:
		VertexBuffer(const void* vertices, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_RendererID;
	};

	class IndexBuffer {
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

}