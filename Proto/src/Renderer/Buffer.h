/*
 * OpenGL 정점 버퍼(VertexBuffer) 및 인덱스 버퍼(IndexBuffer)를 추상화한 클래스입니다.
 * GPU 메모리에 정점 데이터를 할당하고 바인딩하는 역할을 담당합니다.
 */

#pragma once

#include <cstdint>

namespace Proto
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_RendererID;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetCount() const
		{
			return m_Count;
		}

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}