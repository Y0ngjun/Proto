# 진행 상황 (Progress)

## 개요

Proto는 4계층 아키텍처(Core/Scene/Renderer/Editor) 기반의 3D 게임 엔진입니다.
렌더링/에디터 기초 완료. Play Mode 및 데이터 직렬화 진행 중.

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

### Milestone 1: 직렬화 및 에셋 참조 기초 (🔄 현재)
**선행**: Milestone 0 완료
**목표**: 씬 저장/로드 및 UUID 기반 에셋/오브젝트 참조 시스템
- [ ] UUID 시스템 (GameObject 및 에셋 고유 식별)
- [ ] Asset Registry 기초 (UUID ↔ 파일 경로 매핑)
- [ ] Component Serialize/Deserialize 인터페이스
- [ ] SaveScene/LoadScene 완전 구현

### Milestone 2: Play Mode & Runtime (다음)
**선행**: Milestone 1 완료
**목표**: 게임 실행 모드 시작
- [ ] Scene::OnUpdateRuntime() 활성화
- [ ] Runtime 카메라 (SceneCamera) 제어
- [ ] OnRuntimeStart/Stop 콜백
- [ ] 델타 타임 기반 로직 업데이트

### Milestone 3: Physics System (다음)
**선행**: Milestone 2 완료
**목표**: 충돌 및 물리 시뮬레이션
- [ ] Rigidbody Component (속도, 가속도, 중력)
- [ ] Collider Component (AABB, Sphere)
- [ ] 충돌 감지 (Collision Detection)
- [ ] 충돌 콜백 (OnCollisionEnter/Stay/Exit)

### Milestone 4: Scripting & UI (다음)
**선행**: Milestone 3 완료
**목표**: 게임 로직 및 UI 시스템
- [ ] ScriptComponent (OnStart/OnUpdate/OnDestroy)
- [ ] EventBus (게임 이벤트 시스템)
- [ ] UI System (Canvas, Text, Image)
- [ ] Input 통합 (플레이어 제어)

### Milestone 5: Optimization (다음)
**선행**: Milestone 4 완료
**목표**: 대규모 오브젝트 처리 성능 최적화 및 에디터 고도화
- [ ] Object Pool (탄막 게임을 위한 GameObject/컴포넌트 재사용)
- [ ] Asset Manager 고도화 (Asset Registry 기반 메모리 리소스 캐싱)
- [ ] GetComponent 최적화 (O(1) 접근, 정적 타입 ID 방식 등 고려)
- [ ] Game State Manager (MainMenu/Playing/GameOver)
- [ ] 에디터 추가 기능 (콘솔, 파일 탐색기)

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
2. **컴포넌트 직렬화 타입 감지** - 문자열 기반 팩토리 패턴 필요 (현재 런타임은 dynamic_cast RTTI 사용 중)
3. **직렬화 미완료** - Milestone 1에서 구현 필요
4. Editor 콘솔 패널 미구현
5. Editor 파일 탐색기 패널 미구현

---

## ✅ 빌드 상태

- **프로젝트**: Proto.sln (Visual Studio 2022, C++17)
- **상태**: ✅ 빌드 성공 (직렬화 코드 제거)
