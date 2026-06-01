# 진행 상황 (Progress)

Proto: 5계층 아키텍처 기반 3D 엔진 | MVP 목표: 3D 탄막 게임

## 백로그
- 로드맵 재구성
- 숏컷 구현
- 레이아웃 구현 (색깔)
- Camera Component 배경 타입 옵션 추가 (SkyGradient / SolidColor) + 하늘 그라디언트 상수를 Scene 레이어 내 별도 설정 헤더로 분리
- 직렬화/역직렬화 전수조사 (현재 카메라가 Stop 시 역직렬화 안됨)

---

## 완료

- **M0: 기초** — App/Window/GameObject/Component/Renderer/Picking/Gizmo
- **M1: 직렬화** — UUID, YAML 저장/로드, 프로젝트 자동 생성
- **M2: Runtime** — Play Mode, SceneCamera, NativeScript/ScriptRegistry
- **M3: Physics** — Rigidbody, Collider(Box/Sphere), 충돌 감지 및 콜백
- **M4: Scripting** — EventBus, 입력 연동 강화(KeyDown/Up·MouseDelta), 기본 메쉬 생성 단축
- **인프라** — 입력 시스템 분리(RawInput/Input), 에디터 고도화, 단위 테스트 94개
- **에디터 UX** — Scene 뷰포트 하늘/대지 그라디언트 배경 (카메라 방향 연동)
- **하이어라키 계층** — 드래그앤드롭으로 부모-자식 관계 설정/해제, 재귀 렌더링, 월드 변환(자식이 부모 Transform 상속), 직렬화 지원

---

## 로드맵 (우선순위 순)

### 1순위: MVP 완성
- [ ] 플레이어 제어, 탄막 패턴, 적 AI, 게임 로직 (점수/체력)
- [ ] UI System — Canvas, Text, Image (HUD 필수)

### 2순위: 성능 & 안정성
- [ ] GetComponent 최적화 및 에셋 매니저 고도화
- [ ] 머터리얼 캡슐화
- [ ] 외부 물리 엔진 (Jolt/PhysX) 연동

### 3순위: 아키텍처 개선
- [ ] Public API 정립 및 스마트 포인터 전면 도입
- [ ] 중앙 집중식 데이터 수정 시스템 (Command 패턴, Undo/Redo, 프로퍼티 리플렉션)
- [ ] 씬 그리드 고급화
- [ ] 테스트를 위한 컨텍스트 분리 (ImGui, OpenGL 등)

---

## 제한사항 및 빌드

- Windows 전용 (FileDialog), 문자열 기반 컴포넌트 직렬화
- **상태**: 빌드/실행 성공 (`build.ps1` → `Proto.exe`)
