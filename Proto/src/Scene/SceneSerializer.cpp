#include "SceneSerializer.h"
#include "../Core/YAMLHelpers.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"

#include <fstream>
#include <iostream>

namespace Proto {

	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		std::string yamlString = SerializeToString();
		std::ofstream fout(filepath);
		fout << yamlString;
	}

	std::string SceneSerializer::SerializeToString()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (auto& go : m_Scene->GetGameObjects())
		{
			out << YAML::BeginMap; // Entity
			out << YAML::Key << "Entity" << YAML::Value << (uint64_t)go->GetUUID();
			out << YAML::Key << "Name" << YAML::Value << go->GetName();

			out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;
			for (auto& comp : go->GetComponents())
			{
				comp->Serialize(out);
			}
			out << YAML::EndSeq;

			out << YAML::EndMap; // Entity
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
		catch (YAML::ParserException e)
		{
			std::cerr << "Failed to load .scene file: " << e.what() << "\n";
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
		catch (YAML::ParserException e)
		{
			std::cerr << "Failed to load scene from string: " << e.what() << "\n";
			return false;
		}

		return DeserializeNode(data);
	}

	bool SceneSerializer::DeserializeNode(const YAML::Node& data)
	{
		if (!data["Scene"])
			return false;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name = entity["Name"].as<std::string>();

				GameObject* go = m_Scene->CreateGameObject(name);
				go->SetUUID(UUID(uuid));

				auto components = entity["Components"];
				if (components)
				{
					for (auto comp : components)
					{
						std::string compName = comp["Component"].as<std::string>();
						if (compName == "Transform") {
							auto* c = go->GetComponent<Transform>();
							if (!c) c = go->AddComponent<Transform>();
							c->Deserialize(comp);
						}
						else if (compName == "MeshRenderer") {
							auto* c = go->AddComponent<MeshRenderer>();
							c->Deserialize(comp);
						}
						else if (compName == "CameraComponent") {
							auto* c = go->AddComponent<CameraComponent>();
							c->Deserialize(comp);
						}
						else if (compName == "LightComponent") {
							auto* c = go->AddComponent<LightComponent>();
							c->Deserialize(comp);
						}
					}
				}
			}
		}
		return true;
	}
}
