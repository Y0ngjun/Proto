#include <doctest/doctest.h>
#include "../../src/Scene/Scene.h"
#include "../../src/Scene/GameObject.h"
#include "../../src/Scene/Component.h"
#include "../../src/Scene/Components/Transform.h"
#include "../../src/Scene/Components/Rigidbody.h"
#include "../../src/Scene/Components/BoxCollider.h"
#include "../../src/Scene/Components/SphereCollider.h"
#include "../../src/Scene/Components/LightComponent.h"
#include "../../src/Scene/Components/NativeScriptComponent.h"
#include "../../src/Scene/ScriptRegistry.h"

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

	TEST_CASE("Scene Dirty Flag 관리")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;

		// 2. Act & Assert (실행 및 검증)
		CHECK_FALSE(scene.IsDirty()); // 초기 상태는 클린

		scene.SetDirty(true);
		CHECK(scene.IsDirty());

		scene.SetDirty(false);
		CHECK_FALSE(scene.IsDirty());
	}

	TEST_CASE("CreateGameObject 시 자동으로 Dirty 상태가 됨")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;
		CHECK_FALSE(scene.IsDirty());

		// 2. Act (실행)
		scene.CreateGameObject("DirtyTest");

		// 3. Assert (검증)
		CHECK(scene.IsDirty());
	}

	TEST_CASE("RemoveGameObject")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;
		Proto::GameObject* go = scene.CreateGameObject("ToRemove");
		REQUIRE(go != nullptr);
		CHECK(scene.GetGameObjects().size() == 1);

		// 2. Act (실행)
		scene.RemoveGameObject(go);

		// 3. Assert (검증)
		CHECK(scene.GetGameObjects().empty());
	}

	TEST_CASE("GetGameObjectByUUID")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;
		Proto::GameObject* go = scene.CreateGameObject("ByUUID");
		Proto::UUID uuid = go->GetUUID();

		// 2. Act (실행)
		Proto::GameObject* found = scene.GetGameObjectByUUID(uuid);

		// 3. Assert (검증)
		REQUIRE(found != nullptr);
		CHECK(found == go);
		CHECK(found->GetName() == "ByUUID");
	}

	TEST_CASE("GetGameObjectByUUID - 없는 UUID 조회 시 nullptr 반환")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;
		Proto::UUID nonExistentUUID; // 씬에 추가하지 않은 UUID

		// 2. Act (실행)
		Proto::GameObject* found = scene.GetGameObjectByUUID(nonExistentUUID);

		// 3. Assert (검증)
		CHECK(found == nullptr);
	}

	TEST_CASE("다중 GameObject 생성 및 고유 UUID 확인")
	{
		// 1. Arrange (준비)
		Proto::Scene scene;

		// 2. Act (실행)
		Proto::GameObject* go1 = scene.CreateGameObject("Object1");
		Proto::GameObject* go2 = scene.CreateGameObject("Object2");
		Proto::GameObject* go3 = scene.CreateGameObject("Object3");

		// 3. Assert (검증)
		CHECK(scene.GetGameObjects().size() == 3);
		CHECK(go1->GetUUID() != go2->GetUUID());
		CHECK(go2->GetUUID() != go3->GetUUID());
		CHECK(go1->GetUUID() != go3->GetUUID());
	}

	TEST_CASE("GameObject 이름 변경")
	{
		// 1. Arrange (준비)
		Proto::GameObject go("OldName");

		// 2. Act (실행)
		go.SetName("NewName");

		// 3. Assert (검증)
		CHECK(go.GetName() == "NewName");
	}

	TEST_CASE("HasComponent - 컴포넌트 존재 여부 확인")
	{
		// 1. Arrange (준비)
		Proto::GameObject go("TestObject");

		// 2. Act & Assert (실행 및 검증)
		CHECK_FALSE(go.HasComponent<Proto::Transform>());

		go.AddComponent<Proto::Transform>();
		CHECK(go.HasComponent<Proto::Transform>());
	}

	TEST_CASE("RemoveComponent - 컴포넌트 제거")
	{
		// 1. Arrange (준비)
		Proto::GameObject go("TestObject");
		go.AddComponent<Proto::Transform>();
		REQUIRE(go.HasComponent<Proto::Transform>());

		// 2. Act (실행)
		go.RemoveComponent<Proto::Transform>();

		// 3. Assert (검증)
		CHECK_FALSE(go.HasComponent<Proto::Transform>());
	}
}

TEST_SUITE("Transform Component")
{
	TEST_CASE("기본값 확인")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::Transform transform;

		// 2. Assert (검증)
		CHECK(transform.Translation == glm::vec3(0.0f));
		CHECK(transform.Rotation == glm::vec3(0.0f));
		CHECK(transform.Scale == glm::vec3(1.0f));
		CHECK(std::string(transform.GetComponentTypeName()) == "Transform");
	}

	TEST_CASE("Translation 생성자")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::Transform transform(glm::vec3(5.0f, 3.0f, -2.0f));

		// 2. Assert (검증)
		CHECK(transform.Translation == glm::vec3(5.0f, 3.0f, -2.0f));
		CHECK(transform.Scale == glm::vec3(1.0f));
	}

	TEST_CASE("GetTransform - 항등 변환")
	{
		// 1. Arrange (준비)
		Proto::Transform transform;

		// 2. Act (실행)
		glm::mat4 mat = transform.GetTransform();

		// 3. Assert (검증) - 항등 행렬인지 확인
		CHECK(mat == glm::mat4(1.0f));
	}

	TEST_CASE("GetTransform - 이동 변환")
	{
		// 1. Arrange (준비)
		Proto::Transform transform;
		transform.Translation = glm::vec3(1.0f, 2.0f, 3.0f);

		// 2. Act (실행)
		glm::mat4 mat = transform.GetTransform();

		// 3. Assert (검증) - 이동 열(4번째 열) 확인
		CHECK(mat[3][0] == doctest::Approx(1.0f));
		CHECK(mat[3][1] == doctest::Approx(2.0f));
		CHECK(mat[3][2] == doctest::Approx(3.0f));
	}

	TEST_CASE("GetTransform - 균등 배율")
	{
		// 1. Arrange (준비)
		Proto::Transform transform;
		transform.Scale = glm::vec3(2.0f);

		// 2. Act (실행)
		glm::mat4 mat = transform.GetTransform();

		// 3. Assert (검증) - 대각선 스케일 확인
		CHECK(mat[0][0] == doctest::Approx(2.0f));
		CHECK(mat[1][1] == doctest::Approx(2.0f));
		CHECK(mat[2][2] == doctest::Approx(2.0f));
	}
}

TEST_SUITE("Rigidbody Component")
{
	TEST_CASE("기본값 확인")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::Rigidbody rb;

		// 2. Assert (검증)
		CHECK(rb.Velocity == glm::vec3(0.0f));
		CHECK(rb.Acceleration == glm::vec3(0.0f));
		CHECK(rb.Mass == doctest::Approx(1.0f));
		CHECK_FALSE(rb.UseGravity);
		CHECK(rb.Drag == doctest::Approx(0.0f));
		CHECK(std::string(rb.GetComponentTypeName()) == "Rigidbody");
	}

	TEST_CASE("Velocity 수정")
	{
		// 1. Arrange (준비)
		Proto::Rigidbody rb;

		// 2. Act (실행)
		rb.Velocity = glm::vec3(0.0f, -9.81f, 0.0f);

		// 3. Assert (검증)
		CHECK(rb.Velocity == glm::vec3(0.0f, -9.81f, 0.0f));
	}

	TEST_CASE("UseGravity 활성화")
	{
		// 1. Arrange (준비)
		Proto::Rigidbody rb;

		// 2. Act (실행)
		rb.UseGravity = true;
		rb.Mass = 5.0f;
		rb.Drag = 0.1f;

		// 3. Assert (검증)
		CHECK(rb.UseGravity);
		CHECK(rb.Mass == doctest::Approx(5.0f));
		CHECK(rb.Drag == doctest::Approx(0.1f));
	}
}

TEST_SUITE("BoxCollider Component")
{
	TEST_CASE("기본값 확인")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::BoxCollider bc;

		// 2. Assert (검증)
		CHECK(bc.Size == glm::vec3(1.0f));
		CHECK(bc.Offset == glm::vec3(0.0f));
		CHECK(bc.IsTrigger);
		CHECK(std::string(bc.GetComponentTypeName()) == "BoxCollider");
	}

	TEST_CASE("Size 및 Offset 수정")
	{
		// 1. Arrange (준비)
		Proto::BoxCollider bc;

		// 2. Act (실행)
		bc.Size = glm::vec3(2.0f, 1.0f, 3.0f);
		bc.Offset = glm::vec3(0.0f, 0.5f, 0.0f);
		bc.IsTrigger = false;

		// 3. Assert (검증)
		CHECK(bc.Size == glm::vec3(2.0f, 1.0f, 3.0f));
		CHECK(bc.Offset == glm::vec3(0.0f, 0.5f, 0.0f));
		CHECK_FALSE(bc.IsTrigger);
	}
}

TEST_SUITE("SphereCollider Component")
{
	TEST_CASE("기본값 확인")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::SphereCollider sc;

		// 2. Assert (검증)
		CHECK(sc.Radius == doctest::Approx(0.5f));
		CHECK(sc.Offset == glm::vec3(0.0f));
		CHECK(sc.IsTrigger);
		CHECK(std::string(sc.GetComponentTypeName()) == "SphereCollider");
	}

	TEST_CASE("Radius 수정")
	{
		// 1. Arrange (준비)
		Proto::SphereCollider sc;

		// 2. Act (실행)
		sc.Radius = 2.5f;
		sc.IsTrigger = false;

		// 3. Assert (검증)
		CHECK(sc.Radius == doctest::Approx(2.5f));
		CHECK_FALSE(sc.IsTrigger);
	}
}

TEST_SUITE("LightComponent")
{
	TEST_CASE("기본값 확인")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::LightComponent light;

		// 2. Assert (검증)
		CHECK(light.Color == glm::vec3(1.0f));
		CHECK(light.Intensity == doctest::Approx(1.0f));
		CHECK(std::string(light.GetComponentTypeName()) == "LightComponent");
	}

	TEST_CASE("커스텀 색상 및 강도 생성자")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::LightComponent light(glm::vec3(1.0f, 0.5f, 0.0f), 3.0f);

		// 2. Assert (검증)
		CHECK(light.Color == glm::vec3(1.0f, 0.5f, 0.0f));
		CHECK(light.Intensity == doctest::Approx(3.0f));
	}

	TEST_CASE("Intensity 수정")
	{
		// 1. Arrange (준비)
		Proto::LightComponent light;

		// 2. Act (실행)
		light.Intensity = 0.5f;
		light.Color = glm::vec3(0.8f, 0.9f, 1.0f);

		// 3. Assert (검증)
		CHECK(light.Intensity == doctest::Approx(0.5f));
		CHECK(light.Color == glm::vec3(0.8f, 0.9f, 1.0f));
	}
}

// NativeScriptComponent 테스트를 위한 더미 스크립트
namespace
{
	static bool s_OnStartCalled = false;
	static bool s_OnUpdateCalled = false;
	static bool s_OnDestroyCalled = false;

	class TestScript : public Proto::ScriptableEntity
	{
	protected:
		void OnStart() override { s_OnStartCalled = true; }
		void OnUpdate(float) override { s_OnUpdateCalled = true; }
		void OnDestroy() override { s_OnDestroyCalled = true; }
	};
}

TEST_SUITE("NativeScriptComponent")
{
	TEST_CASE("Bind - ScriptName 및 생성 함수 설정")
	{
		// 1. Arrange (준비)
		Proto::NativeScriptComponent nsc;

		// 2. Act (실행)
		nsc.Bind<TestScript>();

		// 3. Assert (검증)
		CHECK_FALSE(nsc.ScriptName.empty());
		CHECK(nsc.InstantiateScript != nullptr);
		CHECK(nsc.DestroyScript != nullptr);
		CHECK(nsc.Instance == nullptr); // OnStart 전에는 Instance 없음
	}

	TEST_CASE("OnStart - 스크립트 인스턴스 생성")
	{
		// 1. Arrange (준비)
		s_OnStartCalled = false;
		Proto::GameObject go("ScriptHost");
		auto* nsc = go.AddComponent<Proto::NativeScriptComponent>();
		nsc->Bind<TestScript>();

		// 2. Act (실행)
		nsc->OnStart();

		// 3. Assert (검증)
		CHECK(nsc->Instance != nullptr);
		CHECK(s_OnStartCalled);
	}

	TEST_CASE("OnUpdate - 스크립트 업데이트 호출")
	{
		// 1. Arrange (준비)
		s_OnStartCalled = false;
		s_OnUpdateCalled = false;
		Proto::GameObject go("ScriptHost");
		auto* nsc = go.AddComponent<Proto::NativeScriptComponent>();
		nsc->Bind<TestScript>();
		nsc->OnStart();

		// 2. Act (실행)
		nsc->OnUpdate(0.016f);

		// 3. Assert (검증)
		CHECK(s_OnUpdateCalled);
	}

	TEST_CASE("OnDestroy - 스크립트 인스턴스 정리")
	{
		// 1. Arrange (준비)
		s_OnDestroyCalled = false;
		Proto::GameObject go("ScriptHost");
		auto* nsc = go.AddComponent<Proto::NativeScriptComponent>();
		nsc->Bind<TestScript>();
		nsc->OnStart();
		REQUIRE(nsc->Instance != nullptr);

		// 2. Act (실행)
		nsc->OnDestroy();

		// 3. Assert (검증)
		CHECK(s_OnDestroyCalled);
		CHECK(nsc->Instance == nullptr);
	}

	TEST_CASE("OnStart 중복 호출 시 인스턴스 재생성 방지")
	{
		// 1. Arrange (준비)
		Proto::GameObject go("ScriptHost");
		auto* nsc = go.AddComponent<Proto::NativeScriptComponent>();
		nsc->Bind<TestScript>();
		nsc->OnStart();
		Proto::ScriptableEntity* firstInstance = nsc->Instance;

		// 2. Act (실행) - 두 번째 OnStart
		nsc->OnStart();

		// 3. Assert (검증) - 인스턴스 교체 없음
		CHECK(nsc->Instance == firstInstance);
	}
}

TEST_SUITE("ScriptRegistry")
{
	TEST_CASE("Register 및 GetScriptNames")
	{
		// 1. Arrange & Act (준비 및 실행)
		Proto::ScriptRegistry::Register<TestScript>("TestScript");
		auto names = Proto::ScriptRegistry::GetScriptNames();

		// 2. Assert (검증)
		bool found = false;
		for (const auto& name : names)
		{
			if (name == "TestScript")
			{
				found = true;
				break;
			}
		}
		CHECK(found);
	}

	TEST_CASE("BindByName - 성공 케이스")
	{
		// 1. Arrange (준비)
		Proto::ScriptRegistry::Register<TestScript>("TestScript");
		Proto::GameObject go("ScriptHost");
		auto* nsc = go.AddComponent<Proto::NativeScriptComponent>();

		// 2. Act (실행)
		Proto::ScriptRegistry::BindByName("TestScript", nsc);

		// 3. Assert (검증)
		CHECK(nsc->ScriptName == "TestScript");
		CHECK(nsc->InstantiateScript != nullptr);
		CHECK(nsc->DestroyScript != nullptr);
	}

	TEST_CASE("BindByName - nullptr 전달 시 크래시 없음")
	{
		// 1. Arrange (준비) - 아무것도 없음

		// 2. Act & Assert (실행 및 검증) - 예외 없이 반환되어야 함
		CHECK_NOTHROW(Proto::ScriptRegistry::BindByName("TestScript", nullptr));
	}

	TEST_CASE("BindByName - 미등록 스크립트 이름")
	{
		// 1. Arrange (준비)
		Proto::GameObject go("ScriptHost");
		auto* nsc = go.AddComponent<Proto::NativeScriptComponent>();

		// 2. Act (실행)
		Proto::ScriptRegistry::BindByName("존재하지않는스크립트", nsc);

		// 3. Assert (검증) - 바인딩되지 않아야 함
		CHECK(nsc->ScriptName.empty());
		CHECK(nsc->InstantiateScript == nullptr);
	}
}
