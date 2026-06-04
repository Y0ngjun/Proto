/*
 * 씬(Scene) 객체의 직렬화 및 역직렬화를 담당하는 클래스입니다.
 * YAML 포맷을 사용하여 씬 내의 게임 오브젝트와 컴포넌트 데이터를 파일 단위로 저장하고 불러옵니다.
 */

#include "SceneSerializer.h"
#include "../Core/YAMLHelpers.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/NativeScriptComponent.h"
#include "../Core/Log.h"
#include "Scene.h"

#include <fstream>

namespace Proto
{
	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		const std::string yamlString = SerializeToString();
		std::ofstream fout(filepath);
		fout << yamlString;
	}

	std::string SceneSerializer::SerializeToString()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (const auto& go : m_Scene->GetGameObjects())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << static_cast<uint64_t>(go->GetUUID());
			out << YAML::Key << "Name" << YAML::Value << go->GetName();
			// 부모 UUID 저장 (루트이면 0)
			const uint64_t parentUUID = go->GetParent()
				? static_cast<uint64_t>(go->GetParent()->GetUUID()) : 0ULL;
			out << YAML::Key << "ParentUUID" << YAML::Value << parentUUID;

			out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;
			for (const auto& comp : go->GetComponents())
			{
				comp->Serialize(out);
			}
			out << YAML::EndSeq;

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		return std::string(out.c_str());
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (const YAML::ParserException& e)
		{
			PROTO_LOG_ERROR("[씬 직렬화 오류] .proto 파일을 로드하는데 실패했습니다: " + std::string(e.what()));
			return false;
		}

		return DeserializeNode(data);
	}

	bool SceneSerializer::DeserializeFromString(const std::string& yamlString)
	{
		YAML::Node data;
		try
		{
			data = YAML::Load(yamlString);
		}
		catch (const YAML::ParserException& e)
		{
			PROTO_LOG_ERROR("[씬 직렬화 오류] 문자열에서 씬을 로드하는데 실패했습니다: " + std::string(e.what()));
			return false;
		}

		return DeserializeNode(data);
	}

	bool SceneSerializer::DeserializeNode(const YAML::Node& data)
	{
		if (!data["Scene"])
		{
			return false;
		}

		const auto entities = data["Entities"];
		if (!entities)
		{
			return true;
		}

		for (const auto& entity : entities)
		{
			const uint64_t uuid = entity["Entity"].as<uint64_t>();
			const std::string name = entity["Name"].as<std::string>();

			GameObject* go = m_Scene->CreateGameObject(name);
			go->SetUUID(UUID(uuid));

			const auto components = entity["Components"];
			if (!components)
			{
				continue;
			}

			for (const auto& comp : components)
			{
				const std::string compName = comp["Component"].as<std::string>();

				if (compName == "Transform")
				{
					auto* c = go->GetComponent<Transform>();
					if (!c)
					{
						c = go->AddComponent<Transform>();
					}
					c->Deserialize(comp);
				}
				else if (compName == "MeshRenderer")
				{
					go->AddComponent<MeshRenderer>()->Deserialize(comp);
				}
				else if (compName == "CameraComponent")
				{
					go->AddComponent<CameraComponent>()->Deserialize(comp);
				}
				else if (compName == "LightComponent")
				{
					go->AddComponent<LightComponent>()->Deserialize(comp);
				}
				else if (compName == "Rigidbody")
				{
					go->AddComponent<Rigidbody>()->Deserialize(comp);
				}
				else if (compName == "BoxCollider")
				{
					go->AddComponent<BoxCollider>()->Deserialize(comp);
				}
				else if (compName == "SphereCollider")
				{
					go->AddComponent<SphereCollider>()->Deserialize(comp);
				}
				else if (compName == "NativeScriptComponent")
				{
					go->AddComponent<NativeScriptComponent>()->Deserialize(comp);
				}
			}
		}

		// --- 2차 패스: 부모-자식 관계 복원 ---
		for (const auto& entity : entities)
		{
			if (!entity["ParentUUID"]) continue;
			const uint64_t parentUUID = entity["ParentUUID"].as<uint64_t>();
			if (parentUUID == 0ULL) continue;
			const uint64_t childUUID = entity["Entity"].as<uint64_t>();
			GameObject* child  = m_Scene->GetGameObjectByUUID(UUID(childUUID));
			GameObject* parent = m_Scene->GetGameObjectByUUID(UUID(parentUUID));
			if (child && parent) m_Scene->SetParent(child, parent);
		}

		return true;
	}
}
