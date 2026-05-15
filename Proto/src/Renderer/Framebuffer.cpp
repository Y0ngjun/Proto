/*
 * 오프스크린 렌더링을 위한 프레임버퍼(Framebuffer) 추상화 클래스입니다.
 * 색상, 깊이, 엔티티 ID 등 여러 렌더링 타겟 어태치먼트를 관리합니다.
 */

#include <glad/glad.h>
#include <iostream>
#include <string>

#include "Framebuffer.h"
#include "../Core/Log.h"

namespace Proto
{
	namespace
	{
		static constexpr uint32_t MAX_FRAMEBUFFER_SIZE = 8192;
		static constexpr uint32_t BUFFER_COUNT_SINGLE = 1;
		static constexpr uint32_t UNBIND_TARGET_ID = 0;
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
		glDeleteTextures(BUFFER_COUNT_SINGLE, &m_ColorAttachment);
		glDeleteTextures(BUFFER_COUNT_SINGLE, &m_EntityIDAttachment);
		glDeleteRenderbuffers(BUFFER_COUNT_SINGLE, &m_DepthAttachment);
	}

	void Framebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
			glDeleteTextures(BUFFER_COUNT_SINGLE, &m_ColorAttachment);
			glDeleteTextures(BUFFER_COUNT_SINGLE, &m_EntityIDAttachment);
			glDeleteRenderbuffers(BUFFER_COUNT_SINGLE, &m_DepthAttachment);
		}

		glGenFramebuffers(BUFFER_COUNT_SINGLE, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color Attachment 0 (RGBA8)
		glGenTextures(BUFFER_COUNT_SINGLE, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// Color Attachment 1 (Entity ID - R32I)
		glGenTextures(BUFFER_COUNT_SINGLE, &m_EntityIDAttachment);
		glBindTexture(GL_TEXTURE_2D, m_EntityIDAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, m_Specification.Width, m_Specification.Height, 0, GL_RED_INTEGER, GL_INT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_EntityIDAttachment, 0);

		// Depth & Stencil Attachment
		glGenRenderbuffers(BUFFER_COUNT_SINGLE, &m_DepthAttachment);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

		const GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			PROTO_LOG_ERROR("프레임버퍼가 불완전합니다!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, UNBIND_TARGET_ID);
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, UNBIND_TARGET_ID);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			PROTO_LOG_WARN("유효하지 않은 크기로 프레임버퍼 크기 조정을 시도했습니다: " + std::to_string(width) + ", " + std::to_string(height));
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		glClearBufferiv(GL_COLOR, attachmentIndex, &value);
	}
}
