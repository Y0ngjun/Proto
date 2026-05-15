/*
 * 엔진 전역에서 고유 식별자로 사용하는 64비트 UUID 클래스입니다.
 * std::random_device를 기반으로 무작위 ID를 생성합니다.
 */

#pragma once

#include <stdint.h>
#include <functional>

namespace Proto
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const
		{
			return m_UUID;
		}

	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<Proto::UUID>
	{
		size_t operator()(const Proto::UUID& uuid) const
		{
			return static_cast<size_t>(uuid);
		}
	};
}
