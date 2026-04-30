# 아키텍처 명세 (Architecture Specification)

## 개요

Proto는 **4계층 아키텍처**를 기반으로 합니다.  
각 계층은 명확한 책임을 가지며, **GameObject-Component 패턴**으로 게임 오브젝트를 관리합니다.

---

## 아키텍처 계층도

```
Application (Singleton)
├── Core
├── Scene
├── Renderer
└── Editor
(GameObject-Component System)  
```

---

## 계층 구조

| 계층 | 디렉토리 | 책임 |
|------|---------|------|
| **Core** | `src/Core/` | 애플리케이션 생명주기, 윈도우, 입력 관리 |
| **Scene** | `src/Scene/` | 게임 객체 및 컴포넌트 관리 |
| **Renderer** | `src/Renderer/` | OpenGL 렌더링 추상화 |
| **Editor** | `src/Editor/` | ImGui 기반 개발 도구 |

---

## 핵심 개념

### GameObject-Component 패턴

```
GameObject = 컨테이너 (ID, Name, Components[])
  ├─ Transform (위치, 회전, 스케일)
  ├─ MeshRenderer (3D 렌더링)
  ├─ CameraComponent
  └─ [Custom Components...]
```

- **GameObject**: 게임 세계의 객체
- **Component**: 기능을 제공하는 모듈
- **확장**: 새로운 Component 상속으로 기능 추가

---

## 게임 루프

```
Application::Run()
  ├─ Init()          (GLFW, OpenGL, ImGui 초기화)
  ├─ Loop()
  │  ├─ Update()     (입력, Scene 업데이트, 물리)
  │  ├─ Render()     (Framebuffer, 렌더링, ImGui)
  │  └─ DeltaTime 계산
  └─ Shutdown()      (리소스 정리)
```

---

## 데이터 흐름

```
Scene::OnUpdateEditor()
  ├─ 각 GameObject 순회
  │  └─ GameObject::Update()
  │     └─ 각 Component::OnUpdate()
  │
  └─ Rendering
     ├─ Framebuffer Bind
     ├─ 각 GameObject 렌더링
     │  └─ Renderer::Submit(Mesh, Shader)
     └─ ImGui Viewport 표시
```

---

## 설계 원칙

1. **GameObject-Component**: 기능 확장이 용이
2. **Singleton (Application)**: 전역 상태 관리
3. **메모리 (unique_ptr)**: 자동 생명주기 관리
4. **에디터/런타임 분리**: 두 가지 Update 메서드
5. **책임 분리**: 각 계층은 자신의 영역만 담당

---

## 디렉토리별 상세 정보

- **`src/Core.md`** - Application, Window, Input
- **`src/Scene.md`** - GameObject, Component, 핵심 컴포넌트
- **`src/Renderer.md`** - Shader, Buffer, Framebuffer, Camera
- **`src/Editor.md`** - ImGui 패널, UI 구성

---

## 다음 확장 계획

- 물리 시스템 (Rigidbody, Collider)
- 스크립트 시스템 (C++ Component)
- 씬 저장/로드 (YAML)
