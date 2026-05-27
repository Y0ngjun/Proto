# CLAUDE.md

Proto는 C++17 기반의 경량 3D 게임 엔진입니다. 비주얼 에디터, 물리/스크립트 시스템을 갖추며 MVP 목표는 3D 탄막 게임입니다.

## 빌드 및 테스트

```powershell
# 빌드
./build.ps1

# 테스트
./run_tests.ps1
```

## 프로젝트 구조

```
src/            # 엔진 소스 코드 (Core / Scene / Renderer / Editor / Asset)
tests/          # 단위 테스트 (doctest 프레임워크)
Resources/      # 런타임 리소스 (셰이더 등)
DefaultProject/ # 기본 프로젝트 파일
Documents/      # 개발 문서
Dependencies/   # 외부 라이브러리 (yaml-cpp, imgui, glfw, glm 등)
```

## 작업 전 필수 참조 문서

모든 작업 시작 전 `Documents/workflow.md`를 읽고 Phase A/B 절차를 따르세요.

| 문서 | 내용 |
|------|------|
| `Documents/workflow.md` | **작업 절차 (Phase A/B)** — 항상 먼저 읽을 것 |
| `Documents/progress.md` | 현재 마일스톤 및 진행 상태 |
| `Documents/spec/architecture.md` | 4계층 아키텍처 개요 |
| `Documents/spec/design.md` | 프로젝트 목표 및 기술 스택 |
| `Documents/style-guide.md` | C++ 네이밍·포맷팅·주석 규칙 |
| `Documents/testing.md` | 테스트 작성(AAA) 및 실행 방법 |
| `Documents/refactoring.md` | 리팩토링 순서 및 지침 |
| `src/Core.md` | Core 계층 상세 구현 |
| `src/Scene.md` | Scene 계층 상세 구현 |
| `src/Renderer.md` | Renderer 계층 상세 구현 |
| `src/Editor.md` | Editor 계층 상세 구현 |
| `src/Asset.md` | Asset 계층 상세 구현 |

## 기본 수칙

### 0. 필수
- **한국어 사용**: 모든 코드 주석과 문서는 한국어로 작성합니다.

### 1. 코딩 전 생각하기
- 가정을 명시적으로 밝히세요. 확실하지 않다면 질문하세요.
- 여러 해석이 가능하다면 조용히 하나를 선택하지 말고 옵션들을 제시하세요.
- 더 간단한 접근 방식이 있다면 말하세요. 불분명하면 멈추고 질문하세요.

### 2. 단순함 우선
- 문제를 해결하는 최소한의 코드만 작성하세요.
- 요청받지 않은 기능, 유연성, 구성 가능성은 추가하지 마세요.

### 3. 국소적 변경
- 꼭 필요한 부분만 건드리세요. 인접한 코드·주석·포맷팅을 "개선"하지 마세요.
- 기존 스타일을 유지하고, 본인 변경으로 불필요해진 것만 제거하세요.

### 4. 목표 주도적 실행
- 성공 기준을 정의하고 검증될 때까지 반복하세요.
- 다단계 작업은 간략한 계획을 먼저 제시하고 승인을 받으세요.
