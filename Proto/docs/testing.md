# 테스트 가이드

Proto 엔진의 테스트 작성 및 실행 방법입니다.

## 테스트 구조

| 타입 | 대상 | 예시 |
|------|------|------|
| **Unit** | 개별 클래스/메서드 | `Transform::SetPosition()` |
| **Integration** | 여러 Component 상호작용 | `GameObject + Transform + Renderer` |
| **System** | 전체 게임 루프 | `Application → Scene → Render` |

**파일 위치**: `src/Test/` → `{Component}Tests.cpp`

---

## Unit Test (AAA 패턴)

**구조**: Arrange (준비) → Act (실행) → Assert (검증)

```cpp
#include <gtest/gtest.h>
#include "../Scene/Components/Transform.h"

namespace Proto::Test
{
    class TransformTest : public ::testing::Test
    {
    protected:
        Transform transform;
    };

    TEST_F(TransformTest, SetPositionUpdatesPosition)
    {
        // Arrange
        glm::vec3 expectedPos(1.0f, 2.0f, 3.0f);

        // Act
        transform.SetPosition(expectedPos);

        // Assert
        EXPECT_EQ(transform.GetPosition(), expectedPos);
    }

    TEST_F(TransformTest, InitialPositionIsZero)
    {
        EXPECT_EQ(transform.GetPosition(), glm::vec3(0.0f));
    }
}
```

## Assertion 매크로

| 매크로 | 용도 |
|--------|------|
| `EXPECT_EQ(a, b)` | 같음 (계속) |
| `ASSERT_EQ(a, b)` | 같음 (중단) |
| `EXPECT_TRUE/FALSE` | 참/거짓 |
| `EXPECT_NEAR(a, b, δ)` | 근사값 |

---

## Integration Test

```cpp
TEST(SceneTest, GameObjectWithComponents)
{
    // GameObject 생성 및 Component 추가
    Proto::Scene scene;
    Proto::GameObject* obj = scene.CreateGameObject("TestObject");
    auto transform = obj->AddComponent<Proto::Transform>();

    // Component 검색 검증
    EXPECT_NE(obj, nullptr);
    EXPECT_EQ(obj->GetComponent<Proto::Transform>(), transform);
}
```

---

## 테스트 실행

### 전체 테스트
```bash
Test > Run All Tests (Visual Studio Test Explorer)
```

### 특정 테스트만
```cpp
run_tests(
    filterTypes: ["TypeName"],
    filterValues: ["Proto::Test::TransformTest"]
)
```

### 실패한 테스트만
```cpp
run_tests(
    filterTypes: ["Outcome"],
    filterValues: ["Failed"]
)
```

---

## 커버리지 기준

| 대상 | 최소 |
|------|------|
| Core (Application) | 80% |
| Scene (GameObject) | 85% |
| Components (Transform) | 90% |
| Renderer | 75% |

---

## 테스트 대상

- ✓ 정상 경로 (Happy Path)
- ✓ 경계값 (Boundary cases)
- ✓ 에러 처리 (Null, Invalid input)
- ✓ 상태 변경 (Edit ↔ Play)
- ✓ Component 상호작용

---

## Mock 예시

```cpp
class MockComponent : public Proto::Component
{
public:
    int updateCount = 0;
    void OnUpdate(float dt) override { updateCount++; }
};

TEST(ComponentTest, LifecycleHooks)
{
    MockComponent mock;
    mock.OnUpdate(0.016f);
    EXPECT_EQ(mock.updateCount, 1);
}
```

---

## 체크리스트

- [ ] 테스트 이름 명확 (`Test{Function}{Scenario}`)
- [ ] AAA 패턴 준수
- [ ] SetUp/TearDown으로 독립성 보장
- [ ] 경계값 테스트 포함
- [ ] 모든 테스트 통과 확인
- [ ] 한국어 주석 (필요시)
