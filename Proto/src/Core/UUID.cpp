/*
 * 엔진 전역에서 고유 식별자로 사용하는 64비트 UUID 클래스입니다.
 * std::random_device를 기반으로 무작위 ID를 생성합니다.
 */

#include <random>

#include "UUID.h"

namespace Proto
{
	namespace
	{
		static std::random_device m_RandomDevice;
		static std::mt19937_64 m_Engine(m_RandomDevice());
		static std::uniform_int_distribution<uint64_t> m_UniformDistribution;
	}

	UUID::UUID()
		: m_UUID(m_UniformDistribution(m_Engine))
	{}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{}
}
