# 코드 스타일 가이드

Proto 엔진의 코드 작성 규칙입니다. 간결함과 일관성을 우선으로 합니다.

## 네이밍 컨벤션

| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스/구조체 | `PascalCase` | `class Application`, `struct Transform` |
| 메서드/함수 | `PascalCase` | `void Run()`, `float GetDeltaTime()` |
| Private 멤버 | `m_` + `PascalCase` | `m_DeltaTime`, `m_SelectedObject` |
| Public 멤버 | `PascalCase` | `int Width`, `float Height` |
| 로컬 변수 | `camelCase` | `deltaTime`, `newObject`, `index` |
| 상수 | `UPPER_SNAKE_CASE` | `MAX_COMPONENTS`, `EPSILON` |
| 네임스페이스 | `PascalCase` | `Proto`, `Proto::Renderer` |

**프로젝트 실제 코드**:
```cpp
// Application.h의 실제 스타일
private:
    float m_DeltaTime;
    float m_LastFrameTime;
    bool m_IsInitialized;
    std::unique_ptr<Framebuffer> m_EditorFramebuffer;
```

---

## 포맷팅

| 항목 | 규칙 |
|------|------|
| 들여쓰기 | **Tab** (1 tab = 4 칸) |
| 중괄호 배치 | **다음 줄** (Allman 스타일) |
| 줄 길이 | 최대 120자 |
| 연산자 간격 | 양쪽 공백 (`a + b`) |

**중괄호 배치 (프로젝트 규칙 준수)**:
```cpp
// ✓ 올바름 (다음 줄)
if (condition)
{
    DoSomething();
}

namespace Proto
{
    class Application
    {
    public:
        void Run()
        {
            // 코드
        }
    };
}
```

---

## 주석

| 대상 | 규칙 |
|------|------|
| 파일 헤더 | 클래스/구조체 파일만 (1-2줄) |
| 메서드 | 복잡한 로직에만 추가 |
| 인라인 | 계산/알고리즘 설명만 |
| 언어 | **한국어만** |

**예시**:
```cpp
// Transform 컴포넌트
// 위치, 회전, 스케일 관리
#pragma once

class Transform : public Component
{
    // 뷰 행렬 역행렬로 월드 좌표 복원
    glm::vec3 worldPos = glm::inverse(viewMatrix) * screenPos;
};

// TODO: 물리 시뮬레이션
// FIXME: 카메라 줌 버그
```

---

## 기본 규칙

### Include 순서
표준 → 외부 라이브러리 → 프로젝트
```cpp
#include <vector>
#include <glm/glm.hpp>
#include "Core/Application.h"
```

### Smart Pointer
```cpp
std::unique_ptr<Framebuffer> fb = std::make_unique<Framebuffer>();
```

### Const / 참조
```cpp
const float EPSILON = 0.0001f;
void SetTransform(const Transform& transform);
```

---

## 체크리스트

- [ ] Private 멤버는 `m_PascalCase`
- [ ] 메서드는 `PascalCase`
- [ ] 중괄호는 **다음 줄** (Allman 스타일)
- [ ] Tab으로 들여쓰기
- [ ] 주석은 한국어
- [ ] Smart Pointer 사용
