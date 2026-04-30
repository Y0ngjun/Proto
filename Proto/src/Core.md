# Core 계층

게임 엔진의 생명주기, 윈도우 관리, 입력 처리를 담당합니다.

## 파일 구조

```
src/Core/
├── Application.h/cpp     (싱글톤 게임 루프 관리)
├── Window.h/cpp          (GLFW 윈도우 관리)
└── Input.h/cpp           (키보드, 마우스 입력)
```

## 주요 클래스

### Application (Singleton)

**책임**: 전체 게임 루프 관리

| 메서드 | 설명 |
|--------|------|
| `Get()` | 싱글톤 인스턴스 반환 |
| `Run()` | 게임 루프 시작 (Init → Loop → Shutdown) |
| `GetDeltaTime()` | 이전 프레임의 경과 시간 반환 |
| `SetScene(Scene*)` | 활성 씬 설정 |
| `SetUpdateCallback()` | 커스텀 업데이트 콜백 설정 |

**내부 구조**:
- `m_Window` - GLFW 윈도우
- `m_EditorFramebuffer / m_GameFramebuffer` - 렌더링 타겟
- `m_EditorCamera` - 에디터 카메라
- `m_SceneState` - Edit 또는 Play 모드
- `m_Scene` - 현재 활성 씬

### Window

**책임**: GLFW 윈도우 및 OpenGL 컨텍스트 관리

| 메서드 | 설명 |
|--------|------|
| `PollEvents()` | GLFW 이벤트 폴링 |
| `SwapBuffers()` | 더블 버퍼 스왑 |
| `ShouldClose()` | 윈도우 종료 요청 확인 |
| `SetVSync(bool)` | 수직 동기화 설정 |
| `GetWidth() / GetHeight()` | 윈도우 크기 반환 |
| `GetNativeWindow()` | 네이티브 GLFWwindow 포인터 |

### Input

**책임**: 키보드, 마우스 입력 처리

| 메서드 | 설명 |
|--------|------|
| `Initialize(GLFWwindow*)` | GLFW 윈도우로 초기화 |
| `GetKey(key)` | 특정 키 눌림 상태 확인 |
| `GetMouseButton(button)` | 마우스 버튼 눌림 상태 확인 |
| `GetMousePosition(x, y)` | 마우스 좌표 반환 |

## 게임 루프 흐름

```
Application::Run()
├── Init()
│   ├── Window 생성 (GLFW)
│   ├── OpenGL 컨텍스트 초기화
│   ├── ImGui 초기화
│   └── Scene 로드
│
├── Loop()
│   ├── PollEvents() (입력 수집)
│   ├── Update()
│   │   ├── Scene::OnUpdateEditor / OnUpdateRuntime
│   │   └── 모든 GameObject::Update()
│   ├── Render()
│   │   ├── Scene 렌더링
│   │   └── ImGui 렌더링
│   └── SwapBuffers()
│
└── Shutdown()
    ├── 리소스 정리
    └── 윈도우 종료
```

## 설계 특징

1. **Singleton Pattern** - 전역 Application 인스턴스 하나
2. **RAII** - Window, Framebuffer 등 리소스 자동 관리
3. **에디터/런타임 분리** - SceneState로 두 모드 구분
4. **콜백 지원** - 커스텀 로직 주입 가능

## 확장 포인트

- `SetUpdateCallback()` - 커스텀 게임 로직 추가
- `ProcessInput()` - 입력 처리 커스터마이징
- SceneState 확장 - 추가 상태 정의 가능
