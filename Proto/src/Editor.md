# Editor 계층

게임 개발의 편의를 돕는 ImGui 기반 UI 도구(Hierarchy, Inspector, Viewport 등)를 제공합니다.

## 핵심 기능 및 패널

1. **SceneHierarchyPanel**
   - 씬에 존재하는 모든 GameObject를 트리 구조로 나열.
   - 객체 선택(Selection) 상태를 중앙에서 관리하여 Inspector에 전달.

2. **InspectorPanel**
   - 선택된 GameObject에 부착된 Component들의 속성을 시각화하고 실시간 수정.
   - Transform, Light 등 값 변경 시 씬에 즉각 반영.

3. **Viewport 및 Gizmo 연동**
   - ImGui 창 내에 게임/에디터 렌더링 결과(Framebuffer 텍스처) 출력.
   - **ImGuizmo**를 활용해 객체의 이동/회전/스케일 시각적 조작.
   - 단축키(Q/W/E/R)를 통한 트랜스폼 모드 전환 지원.

4. **Object Picking**
   - 뷰포트 내 마우스 클릭 위치의 픽셀(Entity ID)을 읽어 특정 GameObject 선택.
   - Gizmo 조작 영역과 겹칠 경우 선택 무시(보호 로직).

## 워크플로우 특징
- **Docking Space**: 여러 패널을 자유롭게 분리/결합할 수 있는 유연한 레이아웃.
- **실시간 반응**: Inspector에서 변경된 수치나 Gizmo의 드래그 결과가 프레임 딜레이 없이 뷰포트에 동기화됨.
