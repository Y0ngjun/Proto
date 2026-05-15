/*
 * OpenGL 정점 배열 객체(Vertex Array Object)를 추상화한 클래스입니다.
 * 정점 버퍼와 인덱스 버퍼를 하나로 묶어 관리하며, 에셋 형태로 취급됩니다.
 */

#include <glad/glad.h>
#include "VertexArray.h"
#include "Buffer.h"

namespace Proto
{
	namespace
	{
		static constexpr uint32_t BUFFER_COUNT_SINGLE = 1;
		static constexpr uint32_t UNBIND_TARGET_ID = 0;

		static constexpr uint32_t POSITION_ATTRIB_INDEX = 0;
		static constexpr uint32_t POSITION_FLOAT_COUNT = 3;

		static constexpr uint32_t NORMAL_ATTRIB_INDEX = 1;
		static constexpr uint32_t NORMAL_FLOAT_COUNT = 3;

		static constexpr int32_t VERTEX_STRIDE = (POSITION_FLOAT_COUNT + NORMAL_FLOAT_COUNT) * sizeof(float);
		static constexpr size_t NORMAL_OFFSET = POSITION_FLOAT_COUNT * sizeof(float);
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(BUFFER_COUNT_SINGLE, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(BUFFER_COUNT_SINGLE, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(UNBIND_TARGET_ID);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		if (!vertexBuffer)
		{
			return;
		}

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		// Position Attribute
		glEnableVertexAttribArray(POSITION_ATTRIB_INDEX);
		glVertexAttribPointer(POSITION_ATTRIB_INDEX, POSITION_FLOAT_COUNT, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, nullptr);

		// Normal Attribute
		glEnableVertexAttribArray(NORMAL_ATTRIB_INDEX);
		glVertexAttribPointer(NORMAL_ATTRIB_INDEX, NORMAL_FLOAT_COUNT, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, reinterpret_cast<void*>(NORMAL_OFFSET));

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		if (!indexBuffer)
		{
			return;
		}

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}