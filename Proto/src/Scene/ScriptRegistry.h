#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <iostream>

namespace Proto
{
	class ScriptableEntity;
	class NativeScriptComponent;

	// 스크립트 생성을 위한 함수 타입 정의
	using ScriptCreatorFunc = std::function<ScriptableEntity*()>;

	class ScriptRegistry
	{
	public:
		// 스크립트 타입을 이름과 함께 등록
		template<typename T>
		static void Register(const std::string& name)
		{
			GetRegistry()[name] = []() { return static_cast<ScriptableEntity*>(new T()); };
			// std::cout << "[ScriptRegistry] Registered script: " << name << std::endl;
		}

		// 이름으로 스크립트 바인딩 정보를 NativeScriptComponent에 설정
		static void BindByName(const std::string& name, NativeScriptComponent* nsc);

		// 등록된 모든 스크립트 이름 목록 반환 (에디터 UI 등에서 사용 가능)
		static std::vector<std::string> GetScriptNames()
		{
			std::vector<std::string> names;
			for (auto const& [name, creator] : GetRegistry())
				names.push_back(name);
			return names;
		}

	private:
		// 정적 맵을 안전하게 초기화하기 위한 헬퍼
		static std::unordered_map<std::string, ScriptCreatorFunc>& GetRegistry()
		{
			static std::unordered_map<std::string, ScriptCreatorFunc> s_Registry;
			return s_Registry;
		}
	};
}
