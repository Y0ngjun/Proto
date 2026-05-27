# CLAUDE.md

Proto는 C++17 기반의 경량 3D 게임 엔진입니다. 비주얼 에디터, 물리/스크립트 시스템을 갖추며 MVP 목표는 3D 탄막 게임입니다.

## 빌드 및 테스트

```powershell
./build.ps1      # 빌드
./run_tests.ps1  # 테스트
```

## 프로젝트 구조

```
src/            # 엔진 소스 코드 (Core / Scene / Renderer / Editor / Asset)
tests/          # 단위 테스트 (doctest 프레임워크)
Resources/      # 런타임 리소스 (셰이더 등)
Documents/      # 개발 문서
Dependencies/   # 외부 라이브러리 (yaml-cpp, imgui, glfw, glm 등)
```

## 참조 문서

| 문서 | 내용 |
|------|------|
| `Documents/progress.md` | 현재 마일스톤 및 진행 상태 |
| `Documents/architecture.md` | 4계층 아키텍처 개요 |
| `Documents/design.md` | 프로젝트 목표 및 기술 스택 |
| `Documents/style_guide.md` | C++ 네이밍·포맷팅·주석 규칙 |
| `Documents/testing.md` | 테스트 작성(AAA) 및 실행 방법 |
| `Documents/refactoring.md` | 리팩토링 순서 및 지침 |
| `src/Core/Core.md` | Core 계층 상세 구현 |
| `src/Scene/Scene.md` | Scene 계층 상세 구현 |
| `src/Renderer/Renderer.md` | Renderer 계층 상세 구현 |
| `src/Editor/Editor.md` | Editor 계층 상세 구현 |
| `src/Asset/Asset.md` | Asset 계층 상세 구현 |

## Phase A: 계획 수립 (항상 먼저 실행)

1. 모호한 점은 즉시 질문.
2. 작업 영역의 참조 문서 확인.
3. 수정할 파일 목록 및 성공 기준 정의.

👉 **계획 제시 후 사용자 승인 대기**

## Phase B: 구현 실행 (승인 후)

1. 필요한 파일 읽기.
2. 구현 실행.
3. 빌드 및 테스트 실행.
4. 관련 `.md` 갱신 및 `Documents/progress.md` 업데이트.

에러 발생 시 원인 분석 후 최소 2가지 해결 옵션을 제시하고 선택을 대기.

## 기본 수칙

- **한국어 사용**: 모든 코드 주석과 문서는 한국어로 작성.
- **질문 우선**: 불확실하거나 해석이 여러 개라면 조용히 선택하지 말고 질문.
- **단순함**: 문제를 해결하는 최소한의 코드만 작성. 요청받지 않은 기능은 추가하지 않음.
- **국소적 변경**: 꼭 필요한 부분만 수정. 인접 코드·포맷팅을 임의로 개선하지 않음.
