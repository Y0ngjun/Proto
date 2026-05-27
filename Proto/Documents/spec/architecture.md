# 아키텍처 명세 (Architecture Specification)

Proto는 **4계층 아키텍처**를 기반으로 하며, **GameObject-Component 패턴**으로 구성됩니다.
상세한 계층별 기능 및 데이터 흐름은 각 `src/*.md` 파일을 참조하세요.

## 1. 아키텍처 계층도
```text
Application (Singleton)
├── Core      : 생명주기, 윈도우(GLFW), 입력 제어 (참조: src/Core.md)
├── Scene     : GameObject 관리, Component 로직 업데이트 (참조: src/Scene.md)
├── Renderer  : OpenGL 추상화, 드로우 콜 처리 (참조: src/Renderer.md)
└── Editor    : ImGui 패널, Gizmo 상호작용 (참조: src/Editor.md)
```

## 2. 핵심 패턴
- **GameObject-Component**: 모든 게임 객체는 ID와 Component(Transform, MeshRenderer 등)의 집합으로 이루어져 있어 유연한 기능 확장이 가능.
- **Singleton**: `Application` 클래스가 전역 상태와 메인 게임 루프 관리.

## 3. 메인 게임 루프 흐름
1. **Init**: 윈도우 생성, OpenGL/ImGui 초기화, 씬 로드.
2. **Loop (Update)**: 입력 수집 → Scene(런타임/에디터) 업데이트 → GameObject내 Component 로직 처리.
3. **Loop (Render)**: Framebuffer 바인딩 → 각 객체의 Renderer Submission → ImGui 렌더링 → 윈도우 버퍼 스왑.
4. **Shutdown**: 사용된 리소스 반환 및 종료.

## 4. 모드 분리
- **Editor Mode**: 편집용 카메라 사용, Gizmo 및 Inspector를 통한 씬 구성.
- **Runtime (Play) Mode**: 물리 시뮬레이션 및 게임 스크립트가 활성화되며 게임 카메라 기준 렌더링.
