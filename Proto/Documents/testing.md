# 테스트 가이드 (Testing Guide)

Proto 엔진의 단위 테스트 작성 및 실행 방법을 설명합니다. 프레임워크는 **doctest**를 사용합니다.

---

## 1. 테스트 실행

프로젝트 루트에서 PowerShell 스크립트를 실행합니다.

```powershell
# 빌드 + 테스트
.\build.ps1 -Tests

# Release 빌드 + 테스트
.\build.ps1 -Config Release -Tests
```

---

## 2. 테스트 작성 요령 (AAA 패턴)

**AAA 패턴 (Arrange - Act - Assert)** 을 준수합니다.

- **Arrange**: 객체 생성 및 데이터 설정
- **Act**: 테스트 대상 메서드/함수 호출
- **Assert**: 결과 검증

```cpp
#include <doctest/doctest.h>
#include "../src/Scene/Scene.h"
#include "../src/Scene/GameObject.h"
#include "../src/Scene/Components/Transform.h"

TEST_SUITE("Scene System")
{
    TEST_CASE("GameObject Creation and Transform Check")
    {
        // Arrange
        Proto::Scene scene;

        // Act
        Proto::GameObject* go = scene.CreateGameObject("TestObject");

        // Assert
        REQUIRE(go != nullptr);
        CHECK(go->GetName() == "TestObject");

        auto transform = go->GetComponent<Proto::Transform>();
        REQUIRE(transform != nullptr);
        CHECK(transform->Translation == glm::vec3(0.0f));
    }
}
```

---

## 3. 주의사항

- 새 `.cpp` 파일 추가 시 `Proto.vcxproj`와 `ProtoTests.vcxproj` **양쪽에 모두 등록**해야 링커 오류가 발생하지 않습니다.
