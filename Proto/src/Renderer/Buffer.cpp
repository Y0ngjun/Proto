/*
 * OpenGL 정점 버퍼(VertexBuffer) 및 인덱스 버퍼(IndexBuffer)를 추상화한 클래스입니다.
 * GPU 메모리에 정점 데이터를 할당하고 바인딩하는 역할을 담당합니다.
 */

#include <glad/glad.h>
#include "Buffer.h"

namespace Proto
{
	namespace
	{
		static constexpr uint32_t BUFFER_COUNT_SINGLE = 1;
		static constexpr uint32_t UNBIND_TARGET_ID = 0;
	}

	// --- VertexBuffer -------------------------------------------------------

	VertexBuffer::VertexBuffer(const void* vertices, uint32_t size)
	{
		glGenBuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, UNBIND_TARGET_ID);
	}

	// --- IndexBuffer --------------------------------------------------------

	IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		glGenBuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, UNBIND_TARGET_ID);
	}
}