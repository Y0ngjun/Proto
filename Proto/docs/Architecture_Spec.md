# 🛠️ Proto Engine Architecture Specification

## 1. Project Overview
- **Project Name**: Proto
- **Goal**: 빠른 게임 로직 검증을 위한 C++ 기반 경량 3D 프로토타이핑 엔진
- **Tech Stack**: C++17, OpenGL (GLFW 3.4, GLAD 3.3), GLM 1.0.3, Dear ImGui (Docking)
- **Architecture**: 4-Layer Architecture (Core -> Renderer -> Scene -> Editor)

---

## 2. Layer 1: Core Engine & Lifecycle (`src/Core`)
- **Application**: 
  - Singleton-based engine controller.
  - Manages initialization (GLFW, ImGui), main game loop (`Run()`), and shutdown.
- **Window**: 
  - Encapsulates GLFW window creation, context management, and swap buffers.
- **Input**: 
  - Singleton manager for hardware input.
  - Polling API: `Input::IsKeyPressed()`, `Input::GetMousePosition()`.

---

## 3. Layer 2: Rendering Pipeline (`src/Renderer`)
- **Goal**: Abstract low-level OpenGL calls into an Object-Oriented C++ API.
- **Shader**: 
  - Handles GLSL compilation, linking, and uniform uploading (via GLM).
- **Buffers**: 
  - `VertexBuffer` (VBO): GPU 메모리에 정점 데이터 업로드.
  - `IndexBuffer` (EBO): 인덱스 데이터 관리 및 드로우 콜 카운트 제공.
  - `VertexArray` (VAO): VBO와 EBO의 상태 및 레이아웃 바인딩 정보 저장.
- **Renderer**: 
  - `Submit(VertexArray, Shader)` 형태의 인터페이스를 제공하여 실제 `glDrawElements` 호출을 캡슐화.

---

## 4. Layer 3: Scene & Component System (`src/Scene`)
- **GameObject-Component Pattern** (Unity-like):
  - **GameObject**: A generic container holding a unique ID, Name, and `std::vector<Component*>`.
  - **Component (Base)**: Abstract class with virtual methods (`OnStart`, `OnUpdate`).
- **Scene Manager**: 
  - Manages the lifecycle of all GameObjects in the current scene.
- **Core Components**:
  - `Transform`: Position, Rotation, Scale (World Matrix = Parent * Local).
  - `MeshRenderer`: Connects to `src/Renderer` to draw 3D primitives (Cube, Plane).
  - `Camera`: Calculates View and Projection matrices.

---

## 5. Layer 4: Editor Interface (`src/Editor`)
- **Dear ImGui Integration**: 
  - Uses Docking branch for flexible multi-panel layout.
- **Hierarchy Panel**: 
  - Visualizes the Scene tree. Supports selection and drag-and-drop parenting.
- **Inspector Panel**: 
  - Reflects the selected GameObject's components. 
  - Allows real-time modification of `Transform` values.
- **Play Mode Control**: 
  - Play/Stop button to test game logic with state backup/restore.

---

## 6. Development Milestones
- **Phase 1 (Core & Renderer)**: 창 띄우기, 입력 처리, 3D 큐브 및 카메라 렌더링.
- **Phase 2 (Scene & Editor)**: GameObject/Component 구조 도입, ImGui 에디터 패널 연동.
- **Phase 3 (Physics & Polish)**: 기초 충돌(AABB), 스크립트 시스템, 최종 데모 빌드.