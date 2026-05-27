#include <doctest/doctest.h>
#include "../../src/Editor/SceneHierarchyPanel.h"
#include "../../src/Editor/EditorLayer.h"
#include "../../src/Scene/Scene.h"
#include "../../src/Scene/GameObject.h"

TEST_SUITE("Editor - SceneHierarchyPanel")
{
    TEST_CASE("기본 생성자: 선택된 오브젝트가 없음")
    {
        // Arrange & Act
        Proto::SceneHierarchyPanel panel;

        // Assert
        CHECK(panel.GetSelectedGameObject() == nullptr);
    }

    TEST_CASE("SetContext: 새 씬으로 변경 시 선택이 초기화됨")
    {
        // Arrange
        Proto::Scene scene1;
        Proto::Scene scene2;
        Proto::SceneHierarchyPanel panel(&scene1);

        Proto::GameObject* go = scene1.CreateGameObject("TestObject");
        panel.SetSelectedGameObject(go);
        REQUIRE(panel.GetSelectedGameObject() == go);

        // Act
        panel.SetContext(&scene2);

        // Assert
        CHECK(panel.GetSelectedGameObject() == nullptr);
    }

    TEST_CASE("SetContext: 동일 씬 재설정 시 선택이 유지됨")
    {
        // Arrange
        Proto::Scene scene;
        Proto::SceneHierarchyPanel panel(&scene);

        Proto::GameObject* go = scene.CreateGameObject("TestObject");
        panel.SetSelectedGameObject(go);

        // Act
        panel.SetContext(&scene);

        // Assert
        CHECK(panel.GetSelectedGameObject() == go);
    }

    TEST_CASE("SetSelectedGameObject / GetSelectedGameObject 라운드트립")
    {
        // Arrange
        Proto::Scene scene;
        Proto::SceneHierarchyPanel panel(&scene);
        Proto::GameObject* go = scene.CreateGameObject("TestObject");

        // Act
        panel.SetSelectedGameObject(go);

        // Assert
        CHECK(panel.GetSelectedGameObject() == go);
    }

    TEST_CASE("SetSelectedGameObject(nullptr): 선택 해제")
    {
        // Arrange
        Proto::Scene scene;
        Proto::SceneHierarchyPanel panel(&scene);
        Proto::GameObject* go = scene.CreateGameObject("TestObject");
        panel.SetSelectedGameObject(go);

        // Act
        panel.SetSelectedGameObject(nullptr);

        // Assert
        CHECK(panel.GetSelectedGameObject() == nullptr);
    }
}

TEST_SUITE("Editor - EditorLayer")
{
    TEST_CASE("기본 기즈모 타입이 TRANSLATE(7)")
    {
        // Arrange & Act
        Proto::EditorLayer editor;

        // Assert
        CHECK(editor.GetGizmoType() == 7);
    }

    TEST_CASE("SetGizmoType / GetGizmoType 라운드트립")
    {
        // Arrange
        Proto::EditorLayer editor;

        // Act
        editor.SetGizmoType(120); // ImGuizmo::ROTATE
        CHECK(editor.GetGizmoType() == 120);

        editor.SetGizmoType(896); // ImGuizmo::SCALE
        CHECK(editor.GetGizmoType() == 896);
    }

    TEST_CASE("초기 게임 뷰 상태: 포커스와 호버 모두 false")
    {
        // Arrange & Act
        Proto::EditorLayer editor;

        // Assert
        CHECK(editor.IsGameViewFocused() == false);
        CHECK(editor.IsGameViewHovered() == false);
    }
}
