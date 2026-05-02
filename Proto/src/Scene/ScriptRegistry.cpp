#include "ScriptRegistry.h"
#include "Components/NativeScriptComponent.h"
#include "../Scene/GameObject.h"
#include "../Core/Log.h"

namespace Proto
{
	void ScriptRegistry::BindByName(const std::string& name, NativeScriptComponent* nsc)
	{
		auto& registry = GetRegistry();
		if (registry.find(name) != registry.end())
		{
			nsc->ScriptName = name;
			nsc->InstantiateScript = registry[name];
			nsc->DestroyScript = [](NativeScriptComponent* nsc)
			{
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
			
			PROTO_LOG_INFO("[ScriptRegistry] Successfully bound script '" + name + "' to GameObject: " + nsc->GetGameObject()->GetName());
		}
		else
		{
			PROTO_LOG_ERROR("[ScriptRegistry] Failed to find script: " + name + ". Make sure it's registered.");
		}
	}
}
