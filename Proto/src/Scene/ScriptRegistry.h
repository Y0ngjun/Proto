/*
 * 네이티브 스크립트 클래스들을 등록하고, 이름(문자열)을 통해 인스턴스를 생성할 수 있도록 도와주는 레지스트리 클래스입니다.
 * 게임 오브젝트에 스크립트를 동적으로 바인딩(Bind)할 때 사용됩니다.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Proto
{
	class ScriptableEntity;
	class NativeScriptComponent;

	using ScriptCreatorFunc = std::function<ScriptableEntity* ()>;

	class ScriptRegistry
	{
	public:
		template<typename T>
		static void Register(const std::string& name)
		{
			GetRegistry()[name] = []()
				{
					return static_cast<ScriptableEntity*>(new T());
				};
		}

		static void BindByName(const std::string& name, NativeScriptComponent* nsc);

		static std::vector<std::string> GetScriptNames()
		{
			std::vector<std::string> names;
			for (const auto& [name, creator] : GetRegistry())
			{
				names.push_back(name);
			}

			return names;
		}

	private:
		ScriptRegistry() = default;
		~ScriptRegistry() = default;

		static std::unordered_map<std::string, ScriptCreatorFunc>& GetRegistry()
		{
			static std::unordered_map<std::string, ScriptCreatorFunc> s_Registry;
			return s_Registry;
		}
	};
}
