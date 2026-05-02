# 진행 상황 (Progress)

## 개요

Proto는 4계층 아키텍처(Core/Scene/Renderer/Editor) 기반의 3D 게임 엔진입니다.
Milestone 3 (Physics System) 완료. Milestone 4 (Scripting & UI) 진행 예정.

---

## 🎯 MVP 목표

**3D 탄막 게임**: 플레이어 조작 → 탄막 회피 → 적 처치 → 게임 오버

필수 엔진 기능: 렌더링 ✅ → Play Mode → Physics → Scripting → UI → 최적화

---

## 📋 Milestone별 로드맵 (의존성 순)

### Milestone 0: 렌더링 & 에디터 기초 ✅
| 계층 | 상태 | 내용 |
|------|------|------|
| Core | ✅ | Application, Window, Input |
| Scene | ✅ | GameObject, Component (Transform, MeshRenderer, Light, Camera) |
| Renderer | ✅ | Shader, VertexArray, Framebuffer, EditorCamera |
| Editor | ✅ | Hierarchy, Inspector, Viewport, Object Picking, Gizmo |

### Milestone 1: 프로젝트 관리 및 직렬화 고도화 ✅
**선행**: Milestone 0 완료
**목표**: 씬 저장/로드 및 프로젝트 단위 관리 시스템 구축
- [x] UUID 시스템 (GameObject 및 에셋 고유 식별)
- [x] Asset Registry 기초 (UUID ↔ 파일 경로 매핑)
- [x] Component Serialize/Deserialize 인터페이스
- [x] SaveScene/LoadScene 완전 구현
- [x] **Project Management System** (프로젝트 독립성 확보, .proto 설정 기반)
- [x] **Default Project Auto-Generation** (엔진 시작 시 기본 프로젝트/씬 자동 생성)
- [x] **Project-centric Workflow** (프로젝트 로드 시 시작 씬 자동 로드 및 폴백 시스템)

### Milestone 2: Play Mode & Runtime ✅
**선행**: Milestone 1 완료
**목표**: 게임 실행 모드 시작
- [x] Scene::OnUpdateRuntime() 활성화
- [x] Runtime 카메라 (SceneCamera) 제어
- [x] OnRuntimeStart/Stop 콜백
- [x] 델타 타임 기반 로직 업데이트
- [x] NativeScriptComponent (OnStart/OnUpdate/OnDestroy, Refactored with ScriptRegistry)
- [x] CameraController 스크립트 (WASDQE 키 이동)

### Milestone 3: Physics System ✅
**선행**: Milestone 2 완료
**목표**: 충돌 및 물리 시뮬레이션
- [x] Rigidbody Component (속도, 가속도, 중력, 공기 저항)
- [x] Collider 추상 기반 클래스 (BoxCollider, SphereCollider 상속)
- [x] O(N²) 충돌 감지 (Box-Box, Sphere-Sphere, Box-Sphere)
- [x] OnCollisionEnter 콜백 (NativeScriptComponent 전용)
- [x] CubeScript 예제 (바닥 충돌 시 Bounce)

### Milestone 4: Scripting & UI (🔄 현재)
**선행**: Milestone 3 완료
**목표**: 게임 로직 심화 및 UI 시스템
- [ ] EventBus (게임 이벤트 시스템)
- [ ] UI System (Canvas, Text, Image)
- [x] ScriptRegistry 도입 (이름 기반 자동 스크립트 바인딩)
- [ ] 플레이어 입력 → 스크립트 연동 강화

### Milestone 5: Optimization & Refinement (다음)
**선행**: Milestone 4 완료
**목표**: 대규모 오브젝트 처리 성능 최적화 및 물리/에디터 시스템 고도화
- [ ] Jolt Physics / PhysX 등 외부 물리 엔진 라이브러리 연동
- [x] 입력 시스템 분리 설계 (Raw Input vs Filtered Game Input)
- [ ] Standalone 빌드 시 GameView 포커스 체크 우회 로직 구현
- [ ] Visual Scripting (Node-based) 시스템 프로토타입 개발
- [ ] Public API (Proto.h) 마스터 헤더 및 네임스페이스 정립
- [ ] Object Pool (탄막 게임을 위한 GameObject/컴포넌트 재사용)
- [ ] Asset Manager 고도화 (Asset Registry 기반 메모리 리소스 캐싱)
- [ ] GetComponent 최적화 (O(1) 접근, 정적 타입 ID 방식 등 고려)
- [ ] Game State Manager (MainMenu/Playing/GameOver)
- [x] 에디터 추가 기능 (콘솔 ✅, 파일 탐색기 ✅)

### Milestone 6: MVP Prototype (마지막)
**선행**: Milestone 5 완료
**목표**: 완성된 탄막 게임
- [ ] 플레이어 (모델, 제어, 충돌)
- [ ] 탄막 생성 및 패턴
- [ ] 적 AI 및 공격
- [ ] 게임 로직 (점수, 체력, GameOver)

---

## 🔴 알려진 제한사항 & TODO

1. **Windows 전용** - FileDialog는 Windows API 사용
2. **컴포넌트 직렬화 타입 감지** - 문자열 기반 팩토리 패턴 사용 (현재 RTTI 없이 컴포넌트 이름으로 분기)

---

## ✅ 빌드 상태

- **프로젝트**: Proto.slnx (Visual Studio 2022, C++17, Debug|x64)
- **빌드**: `build.ps1` → `x64\Debug\Proto.exe`
- **상태**: ✅ 빌드 및 실행 성공 (2026-05-02 기준, 프로젝트 관리 시스템 및 에디터 고도화 완료)
