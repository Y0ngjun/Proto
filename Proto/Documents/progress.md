# 진행 상황 (Progress)

Proto: 4계층 아키텍처 기반 3D 엔진 (Core/Scene/Renderer/Editor)
Milestone 4 (Scripting & UI) 진행 중. MVP 목표: 3D 탄막 게임.

---

## 긴급 안건
- 각 기능 점검
- 테스트 도입

## 백로그
- public API 정의
- 스마트 포인터 전면 도입
- 중앙 집중식 데이터 수정 시스템
  - Command 패턴을 통한 데이터 변경 로직 통일
  - Undo/Redo 기능 구현 기반 구축
  - 프로퍼티 리플렉션
- 씬 그리드 고급화
- 머터리얼 캡슐화
- 입력 시스템 개선 (향후 빌드 고려)
- 테스트를 위한 컨텍스트 분리(ImGui, OpenGL 등)

## 완료된 마일스톤 (Milestone 0~3)
- **M0: 기초**: App/Window/GameObject/Component/Renderer/Picking/Gizmo 완비
- **M1: 직렬화**: UUID, YAML 저장/로드, 프로젝트 자동 생성 및 중심 워크플로우 구축
- **M2: Runtime**: Play Mode, SceneCamera, NativeScript(ScriptRegistry) 구현
- **M3: Physics**: Rigidbody, Collider(Box/Sphere), 충돌 감지 및 콜백 시스템 구축

---

## 진행 중 및 향후 로드맵

### Milestone 4: Scripting & UI (진행 중)
- [ ] EventBus (게임 이벤트 시스템)
- [ ] UI System (Canvas, Text, Image)
- [x] ScriptRegistry (이름 기반 자동 바인딩)
- [x] 기본 메쉬 생성 단축 (Toolbar "Create Primitive" 버튼, Hierarchy 우클릭 메뉴)
- [ ] 플레이어 입력-스크립트 연동 강화

### Milestone 5: Optimization & Refinement (다음)
- [ ] 외부 물리 엔진(Jolt/PhysX) 연동 및 성능 최적화
- [x] 입력 시스템 분리 (Raw Input vs Filtered Game Input)
- [ ] Public API 정립 및 스마트 포인터 전면 도입
- [x] 에디터 고도화 (콘솔 분할 뷰, 스마트 저장/단축키)
- [ ] GetComponent 최적화 및 에셋 매니저 고도화

### Milestone 6: MVP Prototype (마지막)
- [ ] 플레이어 제어, 탄막 패턴, 적 AI, 게임 로직(점수/체력) 구현

---

## 최근 업데이트 (2026-05-27)
- Renderer 계층 단위 테스트 추가: `tests/Renderer/RendererTests.cpp` 생성
  - SceneCamera 9개, EditorCamera 9개 — 총 18개 테스트 케이스 추가
  - OpenGL 컨텍스트 불필요한 순수 수학 클래스만 대상 (Buffer/VAO/Shader/MeshLoader는 백로그 참조)
  - 전체 테스트 56개 통과 (경고 0, 오류 0)

## 최근 업데이트 (2026-05-15)
- Git 루트 `.gitignore` 통합 최적화 (중복 제거, Dependencies 예외 처리, 빌드 결과물 무시 강화)

## 최근 업데이트 (2026-05-14)
- 리팩토링 4사이클 완료: 매직 넘버 상수화, C++ 표준 캐스팅 전환, 한국어 통일, 파일 헤더 정비, 가상 소멸자 추가, 네이티브 스크립트 역직렬화 완성. 빌드 검증 완료 (경고 0, 오류 0)
- 리팩토링 3사이클 완료: 의존성 최소화(전방 선언 활용), Allman 스타일 통일, Early Return 적용, const 강화. 빌드 검증 완료 (경고 0, 오류 0)

---

## 제한사항 및 빌드
- Windows 전용 (FileDialog), 문자열 기반 컴포넌트 직렬화
- **상태**: 빌드/실행 성공 (`build.ps1` -> `Proto.exe`)
