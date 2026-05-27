#include <doctest/doctest.h>
#include "../../src/Scene/Scene.h"
#include "../../src/Scene/GameObject.h"
#include "../../src/Scene/Components/Transform.h"

TEST_SUITE("Scene System")
{
	TEST_CASE("GameObject Creation and Transform Check")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;

		// 2. Act (실행)
		Proto::GameObject* go = scene.CreateGameObject("TestObject");

		// 3. Assert (검증)
		REQUIRE(go != nullptr);
		CHECK(go->GetName() == "TestObject");

		auto transform = go->GetComponent<Proto::Transform>();
		REQUIRE(transform != nullptr); // 생성 시 Transform 컴포넌트 자동 부착 여부 확인
		CHECK(transform->Translation == glm::vec3(0.0f));
		CHECK(transform->Scale == glm::vec3(1.0f));
	}

	TEST_CASE("Component Addition and Retrieval")
	{
		// 1. Arrange (준비)
		Proto::GameObject go("TestObject");

		// 2. Act (실행)
		auto transform = go.AddComponent<Proto::Transform>(glm::vec3(1.0f, 2.0f, 3.0f));

		// 3. Assert (검증)
		REQUIRE(transform != nullptr);
		CHECK(transform->Translation == glm::vec3(1.0f, 2.0f, 3.0f));

		auto retrieved = go.GetComponent<Proto::Transform>();
		REQUIRE(retrieved != nullptr);
		CHECK(retrieved == transform);
	}
}
