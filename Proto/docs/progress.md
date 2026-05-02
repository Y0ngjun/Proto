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

## 📅 최근 업데이트 (2026-05-02)
- **통합 관리**: `Scene::CreateDefault()`로 씬 생성 로직 단일화 및 지연 초기화로 안정성 확보
- **스마트 저장**: Ctrl+S(덮어쓰기) 및 프로젝트 저장 시 현재 씬 자동 저장 연동
- **콘솔 개선**: 유니티 스타일 분할 뷰(상세 로그 확인) 도입 및 가독성 개선
- **단축키**: `GetKeyDown` 구현 및 주요 기능(New/Open/Save) 단축키 연동

---

## 🔴 제한사항 및 빌드
- Windows 전용 (FileDialog), 문자열 기반 컴포넌트 직렬화
- **상태**: ✅ 빌드/실행 성공 (`build.ps1` -> `Proto.exe`)
