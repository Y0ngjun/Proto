#define GLM_ENABLE_EXPERIMENTAL
#include <doctest/doctest.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../../src/Renderer/SceneCamera.h"
#include "../../src/Renderer/EditorCamera.h"

// 부동소수점 비교 허용 오차
static constexpr float EPSILON = 1e-5f;

TEST_SUITE("Renderer - SceneCamera")
{
    TEST_CASE("기본 FOV는 45도(라디안) 값")
    {
        // Arrange & Act
        Proto::SceneCamera cam;

        // Assert
        CHECK(cam.GetPerspectiveVerticalFOV() == doctest::Approx(glm::radians(45.0f)).epsilon(EPSILON));
    }

    TEST_CASE("기본 Near/Far 클립 값 확인")
    {
        // Arrange & Act
        Proto::SceneCamera cam;

        // Assert
        CHECK(cam.GetPerspectiveNearClip() == doctest::Approx(0.1f).epsilon(EPSILON));
        CHECK(cam.GetPerspectiveFarClip() == doctest::Approx(1000.0f).epsilon(EPSILON));
    }

    TEST_CASE("SetViewportSize(0, h) 호출 시 무시됨")
    {
        // Arrange
        Proto::SceneCamera cam;
        const glm::mat4 before = cam.GetProjection();

        // Act
        cam.SetViewportSize(0, 720);

        // Assert
        CHECK(cam.GetProjection() == before);
    }

    TEST_CASE("SetViewportSize 후 투영 행렬이 단위 행렬이 아님")
    {
        // Arrange
        Proto::SceneCamera cam;

        // Act
        cam.SetViewportSize(1280, 720);

        // Assert
        CHECK(cam.GetProjection() != glm::mat4(1.0f));
    }

    TEST_CASE("SetPerspective 후 FOV/Near/Far 값 반영")
    {
        // Arrange
        Proto::SceneCamera cam;
        const float newFOV  = glm::radians(60.0f);
        const float newNear = 0.01f;
        const float newFar  = 500.0f;

        // Act
        cam.SetPerspective(newFOV, newNear, newFar);

        // Assert
        CHECK(cam.GetPerspectiveVerticalFOV() == doctest::Approx(newFOV).epsilon(EPSILON));
        CHECK(cam.GetPerspectiveNearClip()    == doctest::Approx(newNear).epsilon(EPSILON));
        CHECK(cam.GetPerspectiveFarClip()     == doctest::Approx(newFar).epsilon(EPSILON));
    }

    TEST_CASE("SetPerspectiveVerticalFOV 단독 세터")
    {
        // Arrange
        Proto::SceneCamera cam;
        const float newFOV = glm::radians(90.0f);

        // Act
        cam.SetPerspectiveVerticalFOV(newFOV);

        // Assert
        CHECK(cam.GetPerspectiveVerticalFOV() == doctest::Approx(newFOV).epsilon(EPSILON));
    }

    TEST_CASE("SetPerspectiveNearClip 단독 세터")
    {
        // Arrange
        Proto::SceneCamera cam;

        // Act
        cam.SetPerspectiveNearClip(0.05f);

        // Assert
        CHECK(cam.GetPerspectiveNearClip() == doctest::Approx(0.05f).epsilon(EPSILON));
    }

    TEST_CASE("SetPerspectiveFarClip 단독 세터")
    {
        // Arrange
        Proto::SceneCamera cam;

        // Act
        cam.SetPerspectiveFarClip(2000.0f);

        // Assert
        CHECK(cam.GetPerspectiveFarClip() == doctest::Approx(2000.0f).epsilon(EPSILON));
    }

    TEST_CASE("동일 뷰포트 재설정 시 투영 행렬 동일 유지")
    {
        // Arrange
        Proto::SceneCamera cam;
        cam.SetViewportSize(1920, 1080);
        const glm::mat4 first = cam.GetProjection();

        // Act
        cam.SetViewportSize(1920, 1080);

        // Assert
        CHECK(cam.GetProjection() == first);
    }
}

TEST_SUITE("Renderer - EditorCamera")
{
    TEST_CASE("기본 Pitch / Yaw 는 0")
    {
        // Arrange & Act
        Proto::EditorCamera cam;

        // Assert
        CHECK(cam.GetPitch() == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(cam.GetYaw()   == doctest::Approx(0.0f).epsilon(EPSILON));
    }

    TEST_CASE("기본 Distance 는 3")
    {
        // Arrange
        Proto::EditorCamera cam;

        // Act: Distance 는 SetDistance 왕복으로 검증
        cam.SetDistance(3.0f);

        // Assert: 위치가 초기값과 일치
        const glm::vec3 pos = cam.GetPosition();
        CHECK(pos.x == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(pos.y == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(pos.z == doctest::Approx(3.0f).epsilon(EPSILON));
    }

    TEST_CASE("기본 Position 은 (0, 0, 3)")
    {
        // Arrange & Act
        Proto::EditorCamera cam;
        const glm::vec3 pos = cam.GetPosition();

        // Assert
        CHECK(pos.x == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(pos.y == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(pos.z == doctest::Approx(3.0f).epsilon(EPSILON));
    }

    TEST_CASE("SetPitch / GetPitch 라운드트립")
    {
        // Arrange
        Proto::EditorCamera cam;

        // Act
        cam.SetPitch(0.5f);

        // Assert
        CHECK(cam.GetPitch() == doctest::Approx(0.5f).epsilon(EPSILON));
    }

    TEST_CASE("SetYaw / GetYaw 라운드트립")
    {
        // Arrange
        Proto::EditorCamera cam;

        // Act
        cam.SetYaw(1.2f);

        // Assert
        CHECK(cam.GetYaw() == doctest::Approx(1.2f).epsilon(EPSILON));
    }

    TEST_CASE("SetDistance 후 Z 위치가 Distance 와 일치")
    {
        // Arrange
        Proto::EditorCamera cam;

        // Act
        cam.SetDistance(10.0f);
        const glm::vec3 pos = cam.GetPosition();

        // Assert: FocalPoint(0,0,0) - Forward(0,0,-1) * 10 = (0,0,10)
        CHECK(pos.x == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(pos.y == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(pos.z == doctest::Approx(10.0f).epsilon(EPSILON));
    }

    TEST_CASE("SetFocalPoint 후 Position 이 이동함")
    {
        // Arrange
        Proto::EditorCamera cam;
        const glm::vec3 originalPos = cam.GetPosition();

        // Act
        cam.SetFocalPoint({ 5.0f, 0.0f, 0.0f });
        const glm::vec3 newPos = cam.GetPosition();

        // Assert
        CHECK(newPos != originalPos);
        CHECK(newPos.x == doctest::Approx(5.0f).epsilon(EPSILON));
    }

    TEST_CASE("기본 상태 방향 벡터: Forward(0,0,-1), Up(0,1,0), Right(1,0,0)")
    {
        // Arrange & Act
        Proto::EditorCamera cam;
        const glm::vec3 forward = cam.GetForwardDirection();
        const glm::vec3 up      = cam.GetUpDirection();
        const glm::vec3 right   = cam.GetRightDirection();

        // Assert
        CHECK(forward.x == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(forward.y == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(forward.z == doctest::Approx(-1.0f).epsilon(EPSILON));

        CHECK(up.x == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(up.y == doctest::Approx(1.0f).epsilon(EPSILON));
        CHECK(up.z == doctest::Approx(0.0f).epsilon(EPSILON));

        CHECK(right.x == doctest::Approx(1.0f).epsilon(EPSILON));
        CHECK(right.y == doctest::Approx(0.0f).epsilon(EPSILON));
        CHECK(right.z == doctest::Approx(0.0f).epsilon(EPSILON));
    }

    TEST_CASE("GetViewProjection 은 Projection * ViewMatrix 와 일치")
    {
        // Arrange
        Proto::EditorCamera cam(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

        // Act
        const glm::mat4 vp       = cam.GetViewProjection();
        const glm::mat4 expected = cam.GetProjection() * cam.GetViewMatrix();

        // Assert
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                CHECK(vp[col][row] == doctest::Approx(expected[col][row]).epsilon(EPSILON));
            }
        }
    }
}
