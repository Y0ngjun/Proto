/*
 * C++ 코드로 작성된 사용자 정의 스크립트(Native Script)를 게임 오브젝트에 부착하고 관리하는 컴포넌트입니다.
 * ScriptableEntity를 상속받은 커스텀 스크립트 클래스의 인스턴스화, 생명주기, 이벤트를 처리합니다.
 */

#include "NativeScriptComponent.h"
#include "../ScriptRegistry.h"
#include <yaml-cpp/yaml.h>

namespace Proto
{
	void NativeScriptComponent::Reset()
	{
		if (Instance && DestroyScript)
			DestroyScript(this);
		ScriptName        = "";
		InstantiateScript = nullptr;
		DestroyScript     = nullptr;
	}

	void NativeScriptComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "NativeScriptComponent";
		out << YAML::Key << "ScriptName" << YAML::Value << ScriptName;
		out << YAML::Key << "Bound" << YAML::Value << (InstantiateScript != nullptr);
		out << YAML::EndMap;
	}

	void NativeScriptComponent::Deserialize(const YAML::Node& node)
	{
		if (node["ScriptName"])
		{
			const std::string name = node["ScriptName"].as<std::string>();
			ScriptRegistry::BindByName(name, this);
		}
	}
}
