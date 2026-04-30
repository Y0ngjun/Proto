# 진행 상황 (Progress)

## 개요

현재 Proto는 **기본 게임 엔진 핵심**이 완성되었으며, **물리 시스템 및 고급 기능**을 진행 중입니다.

---

## 로드맵

### Phase 1: 기본 렌더링 및 에디터 기초 (✅ 완료)

- ✅ GLFW 윈도우 생성 및 입력 처리
- ✅ OpenGL 3.3 렌더링 파이프라인
- ✅ Shader & Buffer 관리 시스템
- ✅ GameObject-Component 구조
- ✅ Scene 관리
- ✅ ImGui 통합 및 기본 패널 (Hierarchy, Inspector)
- ✅ 에디터 카메라 및 Viewport

**성과**: 큐브 렌더링 테스트 완성, 기본 에디터 환경 구성

---

### Phase 2: 에디터 고도화 및 기본 물리 (🔄 진행 중)

#### 에디터 기능
- ✅ Transform 컴포넌트 실시간 편집 (Gizmo 연동)
- ✅ Gizmo 모드 선택 UI (Inspector 패널)
- ✅ 게임 오브젝트 트리 선택/관리
- ✅ 오브젝트 피킹 (마우스 클릭 선택)
- ✅ Play/Stop 버튼 (런타임 모드)
- ⏳ 씬 그리드 표시

#### 물리 시스템
- ⏳ Rigidbody 컴포넌트 (질량, 속도, 가속도)
- ⏳ Collider 컴포넌트 (AABB 충돌 형태)
- ⏳ 충돌 감지 시스템
- ⏳ 물리 계산 (중력, 힘, 토크)

#### 렌더링 개선
- ⏳ 라이팅 시스템 (기본 Phong)
- ⏳ 법선 맵핑

---

### Phase 3: 데이터 직렬화 및 에셋 관리

- ⏳ 씬 저장/로드 (YAML 포맷)
- ⏳ GameObject 프리팹 (재사용 가능한 템플릿)
- ⏳ Asset Manager (메시, 텍스처, 셰이더 관리)
- ⏳ UUID 시스템 (안정적인 오브젝트 참조)

---

### Phase 4: 스크립트 시스템

- ⏳ C++ 기반 ScriptComponent
- ⏳ 스크립트 라이프사이클 (OnStart, OnUpdate, OnDestroy)
- ⏳ 스크립트 ↔ 게임 엔진 인터페이스

---

### Phase 5: MVP 완성 (3D 탄막 게임 프로토타입)

- ⏳ 플레이어 제어 (이동, 회전)
- ⏳ 탄막 생성 및 발사
- ⏳ 적 AI 기본 구현
- ⏳ 충돌 기반 게임 로직 (HIT, 사망 등)
- ⏳ 게임 상태 관리 (Start, Playing, GameOver)

---

## 현재 구현 상태

### 완료된 기능

#### Core 계층
```
src/Core/
├── ✅ Application (싱글톤, 게임 루프)
├── ✅ Window (GLFW 윈도우 관리)
└── ✅ Input (키보드, 마우스 입력)
```

#### Scene 계층
```
src/Scene/
├── ✅ Scene (GameObject 관리)
├── ✅ GameObject (ID, 이름, Component 컨테이너)
├── ✅ Component (기본 클래스)
└── Components/
    ├── ✅ Transform (위치, 회전, 스케일)
    ├── ✅ MeshRenderer (3D 메시 렌더링)
    ├── ✅ CameraComponent (뷰 설정)
    └── ✅ LightComponent (조명 정보)
```

#### Renderer 계층
```
src/Renderer/
├── ✅ Renderer (정적 렌더링 인터페이스)
├── ✅ Shader (GLSL 컴파일 및 유니폼)
├── ✅ VertexArray (VAO 관리)
├── ✅ Buffer (VBO, IBO)
├── ✅ Framebuffer (오프스크린 렌더링)
├── ✅ EditorCamera (에디터 카메라)
└── ✅ SceneCamera (게임 카메라)
```

#### Editor 계층
```
src/Editor/
├── ✅ SceneHierarchyPanel (GameObject 트리)
├── ✅ InspectorPanel (속성 편집)
└── ✅ Viewport (렌더링 결과 표시)
```

---

### 진행 중인 기능

- ✅ Transform Gizmo 통합 (ImGuizmo) - **완성**
- ✅ Object Picking 완성 - **완성**
- 🔄 런타임 모드 (Play/Stop) - 다음 작업
- ⏳ 씬 그리드 표시

---

### 미구현 기능

#### Phase 2 (예정)
- ⏳ Rigidbody 컴포넌트
- ⏳ Collider 컴포넌트
- ⏳ 충돌 감지 및 물리 계산
- ⏳ 라이팅 시스템

#### Phase 3+
- ⏳ 씬 저장/로드
- ⏳ Asset Manager
- ⏳ UUID 시스템
- ⏳ ScriptComponent
- ⏳ 탄막 게임 로직

---

## 빌드 상태

- **프로젝트**: Proto.sln (Visual Studio 2022)
- **C++ 표준**: C++17
- **테스트**: CubeTest (기본 큐브 렌더링 테스트)
- **빌드 상태**: ✅ 성공

---

## 다음 우선순위

1. **Transform Gizmo** - 에디터에서 직관적 오브젝트 조작
2. **Runtime Mode** - Play/Stop으로 게임 테스트
3. **Physics Basics** - Rigidbody, Collider 구현
4. **Scene Save/Load** - 만든 씬 저장 가능하게

---

## 주요 개발 노트

- GameObject-Component 패턴으로 확장성 확보
- unique_ptr 사용으로 메모리 관리 자동화
- 에디터/런타임 분리로 두 모드 지원
- ImGui Docking으로 유연한 UI 구성
