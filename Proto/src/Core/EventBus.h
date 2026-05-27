/*
 * 타입 안전 Publisher-Subscriber 이벤트 버스입니다.
 * Subscribe로 콜백을 등록하고 Publish로 이벤트를 동기식으로 발행합니다.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace Proto
{
	class EventBus
	{
	public:
		template<typename T>
		uint32_t Subscribe(std::function<void(const T&)> callback)
		{
			uint32_t id = m_NextID++;
			m_Listeners[std::type_index(typeid(T))].push_back(
				{ id, [callback](const void* data)
					{
						callback(*static_cast<const T*>(data));
					}
				});
			return id;
		}

		void Unsubscribe(uint32_t id)
		{
			for (auto& [type, listeners] : m_Listeners)
			{
				listeners.erase(
					std::remove_if(listeners.begin(), listeners.end(),
						[id](const ListenerEntry& e) { return e.ID == id; }),
					listeners.end());
			}
		}

		template<typename T>
		void Publish(const T& event)
		{
			auto it = m_Listeners.find(std::type_index(typeid(T)));
			if (it == m_Listeners.end())
			{
				return;
			}

			for (const auto& entry : it->second)
			{
				entry.Callback(&event);
			}
		}

	private:
		struct ListenerEntry
		{
			uint32_t ID;
			std::function<void(const void*)> Callback;
		};

		std::unordered_map<std::type_index, std::vector<ListenerEntry>> m_Listeners;
		uint32_t m_NextID = 0;
	};
}
