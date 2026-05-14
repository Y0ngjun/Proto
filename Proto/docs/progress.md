# 진행 상황 (Progress)

Proto: 4계층 아키텍처 기반 3D 엔진 (Core/Scene/Renderer/Editor)
Milestone 4 (Scripting & UI) 진행 중. MVP 목표: 3D 탄막 게임.

---

## ✅ 완료된 마일스톤 (Milestone 0~3)
- **M0: 기초**: App/Window/GameObject/Component/Renderer/Picking/Gizmo 완비
- **M1: 직렬화**: UUID, YAML 저장/로드, 프로젝트 자동 생성 및 중심 워크플로우 구축
- **M2: Runtime**: Play Mode, SceneCamera, NativeScript(ScriptRegistry) 구현
- **M3: Physics**: Rigidbody, Collider(Box/Sphere), 충돌 감지 및 콜백 시스템 구축

---

## 📋 진행 중 및 향후 로드맵

### Milestone 4: Scripting & UI (🔄 진행 중)
- [ ] EventBus (게임 이벤트 시스템)
- [ ] UI System (Canvas, Text, Image)
- [x] ScriptRegistry (이름 기반 자동 바인딩)
- [x] 기본 메쉬 생성 단축 (Toolbar "Create Primitive" 버튼, Hierarchy 우클릭 메뉴)
- [ ] 플레이어 입력-스크립트 연동 강화

### Milestone 5: Optimization & Refinement (다음)
- [ ] 외부 물리 엔진(Jolt/PhysX) 연동 및 성능 최적화
- [x] 입력 시스템 분리 (Raw Input vs Filtered Game Input)
- [ ] Public API 정립 및 스마트 포인터 전면 도입
- [x] 에디터 고도화 (콘솔 분할 뷰 ✅, 스마트 저장/단축키 ✅)
- [ ] GetComponent 최적화 및 에셋 매니저 고도화

### Milestone 6: MVP Prototype (마지막)
- [ ] 플레이어 제어, 탄막 패턴, 적 AI, 게임 로직(점수/체력) 구현

---

## 📅 최근 업데이트 (2026-05-03)
- **Phase 5 완료**: Proj/Scene 단위 Save/Open 고도화
  - Proj 단위: NewProject, OpenProject, SaveProject 구현
  - Scene 단위: NewScene, OpenScene, SaveScene, SaveSceneAs 구현
  - 저장 경고: Open/New/Close 시 수정 여부 확인 및 경고 팝업
  - Dirty Flag: Scene/Project에 추가하여 수정 상태 추적
  - 타이틀 바: 윈도우 제목에 "*" 표시로 수정 여부 시각화
  - 단축키: Ctrl+N(New Project), Ctrl+O(Open Project), Ctrl+S(Save Scene), Ctrl+Shift+S(Save Scene As), Alt+F4(Exit with prompt)
  - 메뉴 UI: File 메뉴 정리 및 단축키 연결
  - 경로 관리: Application에서 프로젝트/씬 경로 관리 (파일 이동 안전)

---

## 🔴 제한사항 및 빌드
- Windows 전용 (FileDialog), 문자열 기반 컴포넌트 직렬화
- **상태**: ✅ 빌드/실행 성공 (`build.ps1` -> `Proto.exe`)


