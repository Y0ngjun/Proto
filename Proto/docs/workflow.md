# Copilot 작업 워크플로우

**전체 코딩 원칙**: `.github/copilot-instructions.md` 참조

## 0. 참조 문서 가이드 (작업 전 확인 필수)
작업 상황에 따라 다음 문서를 반드시 참조하세요:

*   **구조 파악 및 기획 (Architecture & Design)**
    *   `docs/spec/architecture.md`: 엔진 4계층 아키텍처 및 데이터 흐름
    *   `docs/spec/design.md`: 프로젝트 타겟, 요구사항, 주요 특징
*   **진행 상태 및 작업 목표 (Status & Tasks)**
    *   `docs/progress.md`: 전체 로드맵 및 현재 구현 상태
    *   `docs/TODO.md`: 단기적 기능 추가 목표
    *   `docs/handoff.md`: 진행 중인 문제와 다음 작업 계획
*   **개발 규칙 (Rules & Guidelines)**
    *   `docs/workflow.md`: 작업 시작 전 계획 수립(Phase A/B) 및 완료 기준 (현재 문서)
    *   `docs/style-guide.md`: C++ 네이밍 컨벤션, 포맷팅, 주석 가이드
    *   `docs/testing.md`: 테스트 작성(AAA 패턴) 및 실행 방법
*   **모듈별 상세 구현 (Layer Details)** - 해당 영역 코드 수정 시 참조
    *   `src/Core.md`: Application 게임 루프, Window, Input
    *   `src/Scene.md`: GameObject, Component 시스템 데이터 흐름
    *   `src/Renderer.md`: OpenGL 렌더링, Shader, Camera 파이프라인
    *   `src/Editor.md`: ImGui 기반 패널, Gizmo, Object Picking

## Phase A: 계획 수립 (항상 먼저 실행)
1. 요구사항 이해: 모호한 점이 있다면 즉시 질문.
2. 참조 문서 확인: 작업 영역에 맞는 위 가이드의 문서를 확인.
3. 영향 범위 파악: 수정할 파일 목록 정리.
4. 구현 순서 결정 및 성공 기준 정의.
👉 **계획 제시 후 사용자 확인(승인) 대기**

## Phase B: 구현 실행 (사용자 승인 후)
1. 코드 컨텍스트 확보: 필요한 파일들 읽기.
2. 코드 수정: 안전하고 최소한의 변경(Surgical Changes) 적용.
3. 검증: 빌드 및 테스트(테스트 가이드는 `docs/testing.md` 참고) 실행.
4. 문서 업데이트: 변경된 내용이 있다면 관련 `.md` 갱신.

## 작업 타입별 절차
| 타입 | 절차 |
|------|------|
| 🐛 **버그** | Phase A (원인분석) → 승인 대기 → Phase B (수정, 테스트) |
| ✨ **기능** | Phase A (요구사항, 위치) → 승인 대기 → Phase B (구현, 테스트) |
| 🔄 **리팩** | Phase A (범위, 안전성) → 승인 대기 → Phase B (수정, 회귀테스트) |

## 문제 발생 시 대처
- 예상치 못한 에러/문제가 발생하면 원인을 분석하고 최소 2가지 해결 옵션을 제시.
- 사용자의 선택을 대기한 후 선택안으로 진행.

## 완료 기준
- 빌드 성공 및 테스트 통과
- 진행 상황에 따라 `docs/progress.md` 또는 `docs/handoff.md` 갱신
- 결과 요약 제공
