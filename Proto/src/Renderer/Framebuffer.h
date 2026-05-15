/*
 * 오프스크린 렌더링을 위한 프레임버퍼(Framebuffer) 추상화 클래스입니다.
 * 색상, 깊이, 엔티티 ID 등 여러 렌더링 타겟 어태치먼트를 관리합니다.
 */

#pragma once

#include <cstdint>

namespace Proto
{
	struct FramebufferSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		~Framebuffer();

		void Invalidate();

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);
		int ReadPixel(uint32_t attachmentIndex, int x, int y);

		void ClearAttachment(uint32_t attachmentIndex, int value);

		uint32_t GetColorAttachmentRendererID() const
		{
			return m_ColorAttachment;
		}

		const FramebufferSpecification& GetSpecification() const
		{
			return m_Specification;
		}

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_EntityIDAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;
	};
}
