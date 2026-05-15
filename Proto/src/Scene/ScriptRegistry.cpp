/*
 * 네이티브 스크립트 클래스들을 등록하고, 이름(문자열)을 통해 인스턴스를 생성할 수 있도록 도와주는 레지스트리 클래스입니다.
 * 게임 오브젝트에 스크립트를 동적으로 바인딩(Bind)할 때 사용됩니다.
 */

#include "ScriptRegistry.h"
#include "Components/NativeScriptComponent.h"
#include "../Scene/GameObject.h"
#include "../Core/Log.h"

namespace Proto
{
	void ScriptRegistry::BindByName(const std::string& name, NativeScriptComponent* nsc)
	{
		if (!nsc)
		{
			return;
		}

		auto& registry = GetRegistry();
		const auto it = registry.find(name);

		if (it == registry.end())
		{
			PROTO_LOG_ERROR("[스크립트 레지스트리 오류] 스크립트를 찾을 수 없습니다: " + name + ". 등록되었는지 확인하세요.");
			return;
		}

		nsc->ScriptName = name;
		nsc->InstantiateScript = it->second;
		nsc->DestroyScript = [](NativeScriptComponent* component)
			{
				delete component->Instance;
				component->Instance = nullptr;
			};

		PROTO_LOG_INFO("[스크립트 레지스트리 정보] 스크립트 '" + name + "' 가 성공적으로 바인딩되었습니다. 대상: " + nsc->GetGameObject()->GetName());
	}
}
