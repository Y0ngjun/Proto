/*
 * 씬(Scene) 객체의 직렬화 및 역직렬화를 담당하는 클래스입니다.
 * YAML 포맷을 사용하여 씬 내의 게임 오브젝트와 컴포넌트 데이터를 파일 단위로 저장하고 불러옵니다.
 */

#pragma once

#include <memory>
#include <string>

namespace YAML
{
	class Node;
}

namespace Proto
{
	class Scene;

	class SceneSerializer
	{
	public:
		SceneSerializer(Scene* scene);

		void Serialize(const std::string& filepath);
		bool Deserialize(const std::string& filepath);

		std::string SerializeToString();
		bool DeserializeFromString(const std::string& yamlString);

	private:
		bool DeserializeNode(const YAML::Node& data);

		Scene* m_Scene;
	};
}
