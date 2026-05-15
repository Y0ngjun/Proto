/*
 * 게임 오브젝트에 부착되어 해당 위치/회전에서 씬을 렌더링하는 카메라 컴포넌트입니다.
 * 주 카메라(Primary) 여부 및 고정 종횡비 옵션을 포함합니다.
 */

#include "CameraComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <yaml-cpp/yaml.h>

namespace Proto
{
	namespace
	{
		static constexpr float DEFAULT_NEAR_CLIP = 0.1f;
		static constexpr float DEFAULT_FAR_CLIP = 1000.0f;
	}

	void CameraComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "CameraComponent";
		out << YAML::Key << "Primary" << YAML::Value << Primary;
		out << YAML::Key << "FixedAspectRatio" << YAML::Value << FixedAspectRatio;

		// 카메라 설정 직렬화
		out << YAML::Key << "FOV" << YAML::Value << Camera.GetPerspectiveVerticalFOV();
		out << YAML::Key << "NearClip" << YAML::Value << Camera.GetPerspectiveNearClip();
		out << YAML::Key << "FarClip" << YAML::Value << Camera.GetPerspectiveFarClip();

		out << YAML::EndMap;
	}

	void CameraComponent::Deserialize(const YAML::Node& node)
	{
		if (node["Primary"])
		{
			Primary = node["Primary"].as<bool>();
		}

		if (node["FixedAspectRatio"])
		{
			FixedAspectRatio = node["FixedAspectRatio"].as<bool>();
		}

		// 카메라 설정 복원
		if (node["FOV"])
		{
			const float fov = node["FOV"].as<float>();
			const float nearClip = node["NearClip"] ? node["NearClip"].as<float>() : DEFAULT_NEAR_CLIP;
			const float farClip = node["FarClip"] ? node["FarClip"].as<float>() : DEFAULT_FAR_CLIP;

			Camera.SetPerspective(fov, nearClip, farClip);
		}
	}
}
