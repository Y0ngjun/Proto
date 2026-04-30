# Editor 계층

게임 개발 환경을 위한 ImGui 기반 에디터 UI와 도구를 제공합니다.

## 파일 구조

```
src/Editor/
├── SceneHierarchyPanel.h/cpp  (씬 계층 구조 트리)
└── InspectorPanel.h/cpp        (선택된 오브젝트 속성)
```

## 주요 클래스

### SceneHierarchyPanel

**책임**: 씬 내 GameObject 계층 표시 및 선택

| 메서드 | 설명 |
|--------|------|
| `SetContext(Scene*)` | 표시할 씬 설정 |
| `OnImGuiRender()` | 매 프레임 UI 렌더링 |
| `GetSelectedGameObject()` | 선택된 GameObject 반환 |
| `SetSelectedGameObject()` | GameObject 선택 설정 |

**UI 구조**:
```
SceneHierarchyPanel
└── GameObject 트리
    ├── MainCamera
    ├── Cube
    └── Light (향후)
```

**기능**:
- 씬의 모든 GameObject 나열
- 트리 구조로 계층 표시
- 클릭으로 GameObject 선택
- 드래그/드롭 (향후 예정)

### InspectorPanel

**책임**: 선택된 GameObject의 Component 검사 및 수정

| 메서드 | 설명 |
|--------|------|
| `OnImGuiRender(selected)` | 선택된 GameObject의 컴포넌트 표시 |

**UI 구조**:
```
InspectorPanel
├── Gizmo 컨트롤
│   ├── None (비활성화)
│   ├── Move (이동) - 단축키: W
│   ├── Rot (회전) - 단축키: E
│   └── Scl (스케일) - 단축키: R
├── GameObject 정보
│   ├── Name
│   └── ID
└── Component 리스트
    ├── Transform
    │   ├── Position (x, y, z)
    │   ├── Rotation (x, y, z)
    │   └── Scale (y, y, z)
    ├── MeshRenderer
    │   ├── Mesh 선택
    │   └── Shader 선택
    └── [Other Components...]
```

**기능**:
- ✅ Gizmo 모드 선택 (UI 버튼)
- ✅ 실시간 Gizmo 모드 표시
- ✅ 컴포넌트별 속성 검사
- ✅ 실시간 속성 수정
- ⏳ 컴포넌트 추가/삭제
- ⏳ 리셋, 복사 기능

## 에디터 UI 레이아웃

```
┌─────────────────────────────────────┐
│  Menu Bar (File, Edit, View)        │
├──────────────┬──────────────┬───────┤
│  Hierarchy   │   Viewport   │ Insp. │
│  Panel       │   (Scene)    │ Panel │
│              │              │       │
│ - MainCamera │ [3D View]    │ Name: │
│ - Cube       │ with Grid    │ Pos:  │
│ - Light      │              │ Rot:  │
│              │              │ Scl:  │
├──────────────┴──────────────┴───────┤
│  Output/Console Panel               │
└─────────────────────────────────────┘
```

## 에디터 워크플로우

```
1. Scene 로드
   └── SceneHierarchyPanel::SetContext(scene)

2. GameObject 선택
   ├─ Hierarchy 트리 클릭 (항상 가능)
   └─ Viewport 클릭 (Gizmo 위 제외)
       ├─ Gizmo 위: 선택 무시 (조작 방지)
       ├─ 오브젝트 위: ID로 선택
       └─ 빈 공간: 선택 해제

3. Transform 편집 (Gizmo 이용)
   ├─ Inspector에서 모드 선택 (또는 Q/W/E/R 키)
   ├─ 뷰포트에 Gizmo 표시
   └─ 마우스로 오브젝트 조작
       ├─ Gizmo 위 클릭: 조작 시작 (다른 오브젝트 선택 안 됨)
       ├─ 이동: 빨강(X), 초록(Y), 파랑(Z) 축 드래그
       ├─ 회전: 원형 축 드래그
       └─ 스케일: 끝단 큐브 드래그

   **고급**: Ctrl 누르면 Snap 활성화 (이동 0.5, 회전 45°)

4. Inspector에서 직접 수정
   └─ InspectorPanel::OnImGuiRender(selected)
       ├─ Transform 값 수정
       ├─ ImGui Slider/Drag 입력
       └─ Transform 값 업데이트 (Gizmo도 함께 변경)

5. 뷰포트 업데이트
   └─ EditorCamera 조작
       ├─ 마우스 중버튼 드래그 - 회전
       ├─ 휠 - 줌
       └─ 우클릭 + WASD - 이동
```

## Gizmo 사용 가이드

| 작업 | 방법 |
|------|------|
| **모드 선택** | Inspector의 버튼 또는 단축키 (Q/W/E/R) |
| **이동 (Move)** | W 키 또는 Inspector 버튼 |
| **회전 (Rotate)** | E 키 또는 Inspector 버튼 |
| **스케일 (Scale)** | R 키 또는 Inspector 버튼 |
| **비활성화** | Q 키 또는 None 버튼 |
| **Snap 모드** | Ctrl 키 누른 상태에서 조작 |

**마우스 조작**:
- 축(X/Y/Z) 드래그: 해당 축만 변경
- 면(XY/YZ/XZ) 드래그: 두 축 동시 변경
- 중심 드래그: 모든 축 변경

## Object Picking 구현

**원리:**
- Framebuffer의 Entity ID 텍스처 기반
- 마우스 클릭 → 해당 픽셀의 GameObject ID 읽기
- Gizmo 위에서는 선택 무시 (오브젝트 조작 보호)

**동작:**
```cpp
if (마우스 클릭 && Viewport 안 && !Gizmo 위)
{
    PixelID = ReadPixel(mouseX, mouseY);
    GameObject = Scene::GetGameObjectByID(PixelID);
    SelectGameObject(GameObject);
}
```

**안전성:**
- `ImGuizmo::IsOver()`: Gizmo UI 어디든 클릭 무시
- Gizmo가 오브젝트 밖으로 나가도 정상 작동
- 의도치 않은 오브젝트 선택 방지

## ImGui 통합

**설정**:
- Docking 활성화
- 다중 패널 자유 배치
- Drag & Drop 지원 준비

**패널 상태**:
- 각 패널은 독립적 윈도우
- 레이아웃 저장/복원 가능 (향후)
- 단축키 지원 (향후)

## 설계 특징

1. **ImGui 기반** - 즉시모드(Immediate Mode) UI
2. **Panel 패턴** - 독립적인 UI 모듈화
3. **Docking** - 유연한 레이아웃
4. **실시간 편집** - 변경사항 즉시 반영

## 확장 포인트

- ✅ Transform Gizmo - 이동/회전/스케일 조작 (완성)
- ✅ Object Picking - 마우스 클릭으로 오브젝트 선택 (완성)
- 🔄 Play/Stop 버튼 - 런타임 모드 전환 (다음)
- ⏳ 씬 그리드 표시 - 정렬 도움
- ⏳ 저장/로드 - 씬 직렬화
- ⏳ 애니메이션 타임라인 - 키프레임 에디터
- ⏳ Asset 브라우저 - 메시, 셰이더 선택
- ⏳ Output 콘솔 - 디버그 로그 표시
