# Editor Layer

Editor 계층은 게임 개발 프로세스를 효율화하기 위한 ImGui 기반의 시각적 도구 모음입니다. 엔진의 상태를 실시간으로 모니터링하고 수정할 수 있는 다양한 패널과 조작 인터페이스를 제공합니다.

## 📂 파일 구조
- `src/Editor/SceneHierarchyPanel.h/cpp`: 씬 객체 트리 구조 및 선택 관리
- `src/Editor/InspectorPanel.h/cpp`: 선택된 객체의 컴포넌트 속성 편집
- `src/Editor/ConsolePanel.h/cpp`: 엔진 로그 출력 및 필터링
- `src/Editor/ContentBrowserPanel.h/cpp`: 에셋 및 파일 시스템 탐색
- `src/Core/Application.cpp`: (Editor Logic 포함) DockSpace, Viewport, Gizmo, Toolbar 통합 관리

---

## 🛠 핵심 UI 컴포넌트

### 1. Docking Space & Layout
에디터의 유연한 작업 환경을 위해 ImGui Docking 시스템을 사용합니다.
- **Dynamic Layout**: 패널들을 자유롭게 드래그하여 분리하거나 결합할 수 있습니다.
- **Fixed Panels**: Scene View, Game View, Hierarchy, Inspector가 기본 레이아웃으로 설정됩니다.

### 2. Toolbar (Play/Stop Control)
엔진의 실행 상태를 제어하는 중앙 제어 바입니다.
- **Scene State**: `Edit` 모드와 `Play` 모드를 전환합니다.
- **State Preservation**: Play 모드 진입 시 현재 씬의 상태를 메모리에 백업하고, Stop 시 백업된 데이터를 복원하여 에디터 상태를 유지합니다.

### 3. Scene Hierarchy Panel
현재 활성화된 씬에 존재하는 모든 `GameObject`를 계층 구조로 시각화합니다.
- **Selection**: 객체를 클릭하여 전역 선택 상태를 업데이트하며, 이는 Inspector 패널과 동기화됩니다.
- **Search & Filter**: (향후 예정) 수많은 객체 중 원하는 대상을 빠르게 찾을 수 있는 기능을 제공합니다.

### 4. Inspector Panel
선택된 객체의 상세 데이터를 확인하고 편집합니다.
- **Component Visualization**: `Transform`, `MeshRenderer`, `Camera`, `Light` 등 부착된 모든 컴포넌트의 유니폼/변수들을 실시간으로 수정할 수 있습니다.
- **Reflective UI**: 객체 지향적인 방식으로 컴포넌트 추가 및 데이터 수정을 처리합니다.

### 5. Console Panel
엔진 내부에서 발생하는 각종 메시지를 실시간으로 출력합니다.
- **Log Levels**: Info(청색), Warning(황색), Error(적색)로 구분하여 시각적인 가독성을 제공합니다.
- **Utility**: Clear 기능을 통해 로그를 비우거나, Auto-scroll 기능을 제어할 수 있습니다.

### 6. Content Browser Panel
프로젝트의 에셋 디렉토리를 탐색하고 관리하는 창입니다.
- **File System Navigation**: `assets` 폴더를 기준으로 실제 파일 시스템을 탐색하며, 폴더 진입 및 뒤로 가기 기능을 지원합니다.
- **Grid View**: 아이콘 형태의 그리드 레이아웃을 통해 직관적인 에셋 확인이 가능합니다.

---

## 🕹 조작 및 상호작용 (Interaction)

### 1. Viewport & ImGuizmo
게임 월드를 직접 조작할 수 있는 메인 창입니다.
- **Gizmo Control**: `ImGuizmo`를 연동하여 객체의 위치(Move), 회전(Rotate), 크기(Scale)를 시각적으로 조작합니다.
- **Shortcuts**: `Q`(None), `W`(Move), `E`(Rotate), `R`(Scale) 단축키를 통해 조작 모드를 즉시 전환합니다.
- **Snapping**: `Left Control` 키를 조합하여 정해진 단위로 정밀하게 조작할 수 있습니다.

### 2. Object Picking (Pixel Perfect)
뷰포트 내의 객체를 마우스로 직접 클릭하여 선택하는 시스템입니다.
- **Framebuffer ID**: 렌더링 시 보이지 않는 별도의 버퍼(Attachment 1)에 각 객체의 고유 ID를 기록합니다.
- **Coordinate Mapping**: 마우스 좌표를 프레임버퍼 픽셀 좌표로 변환하여 해당 위치의 ID를 읽어냄으로써 정확한 객체를 선택합니다.

---

## 🎨 디자인 원칙
*   **Non-Intrusive**: 에디터 UI 로직이 실제 게임 런타임 코드에 영향을 주지 않도록 최대한 분리하여 설계합니다.
*   **Intuitive Feedback**: 사용자의 모든 조작(값 변경, 기즈모 드래그 등)은 즉각적으로 뷰포트에 반영되어야 합니다.
*   **Extensibility**: 새로운 컴포넌트나 패널을 추가하기 쉬운 플러그형 구조를 지향합니다.
