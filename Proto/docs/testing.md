# 테스트 가이드

## 테스트 구조

| 타입 | 대상 | 위치 |
|------|------|------|
| **Unit** | 개별 클래스/메서드 | `src/Test/` |
| **Integration** | 여러 Component 상호작용 | `src/Test/` |
| **System** | 전체 게임 루프 | `src/Test/` |

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

## 테스트 실행

### 전체 테스트
```bash
Test > Run All Tests (Visual Studio Test Explorer)
```

### 특정 테스트
```bash
run_tests(filterTypes: ["TypeName"], filterValues: ["Proto::Test::TransformTest"])
```

---

## 테스트 대상

- ✓ 정상 경로 (Happy Path)
- ✓ 경계값 (Boundary cases)
- ✓ 에러 처리 (Null, Invalid input)
- ✓ Component 상호작용

---

## 체크리스트

- [ ] 테스트 이름 명확 (`Test{Function}{Scenario}`)
- [ ] AAA 패턴 준수
- [ ] 경계값 테스트 포함
- [ ] 모든 테스트 통과 확인
