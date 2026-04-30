# Scene 계층

게임 객체(GameObject)와 컴포넌트(Component) 시스템을 관리합니다.

## 파일 구조

```
src/Scene/
├── Scene.h/cpp                (씬 및 GameObject 관리)
├── GameObject.h/cpp           (게임 객체 컨테이너)
├── Component.h                (컴포넌트 기본 클래스)
└── Components/
    ├── Transform.h/cpp        (위치, 회전, 스케일)
    ├── MeshRenderer.h         (메시 렌더링)
    ├── CameraComponent.h      (카메라 설정)
    └── LightComponent.h       (조명 정보)
```

## 주요 클래스

### Scene

**책임**: GameObject 생명주기 관리

| 메서드 | 설명 |
|--------|------|
| `CreateGameObject(name)` | 새로운 GameObject 생성 |
| `OnUpdateRuntime(dt)` | 게임 실행 중 업데이트 |
| `OnUpdateEditor(dt, camera)` | 에디터 모드 업데이트 |
| `OnRuntimeStart()` | 게임 시작 시점 |
| `OnRuntimeStop()` | 게임 중지 시점 |
| `GetGameObjects()` | 모든 GameObject 반환 |
| `GetGameObjectByID(id)` | ID로 GameObject 검색 |

### GameObject

**책임**: Component 컨테이너

| 메서드 | 설명 |
|--------|------|
| `GetID()` | 고유 ID 반환 |
| `GetName()` | 이름 반환 |
| `Update(dt)` | 모든 Component 업데이트 |
| `AddComponent<T>(args...)` | Component 추가 및 OnStart() 호출 |
| `GetComponent<T>()` | Component 타입으로 검색 |

**내부 구조**:
- `m_ID` - 고유 식별자 (자동 증가)
- `m_Name` - 디버그용 이름
- `m_Components` - Component 벡터 (unique_ptr)

### Component

**책임**: 기본 컴포넌트 인터페이스

| 메서드 | 설명 |
|--------|------|
| `OnStart()` | GameObject 추가 시 호출 |
| `OnUpdate(dt)` | 매 프레임 호출 |
| `OnDestroy()` | GameObject 삭제 시 호출 |
| `GetGameObject()` | 소속 GameObject 반환 |

## 핵심 컴포넌트들

### Transform

3D 변환 정보 (위치, 회전, 스케일)

```
Translation (glm::vec3)  - 월드 좌표
Rotation (glm::vec3)     - 오일러 각 (도)
Scale (glm::vec3)        - 스케일 배수

GetTransform()           - 월드 행렬 계산
```

### MeshRenderer

3D 메시 렌더링

```
SetMesh(VAO)             - 메시 설정
SetShader(Shader)        - 셰이더 설정
GetMesh() / GetShader()  - 데이터 조회
```

### CameraComponent

카메라 설정

```
GetViewMatrix()          - 뷰 행렬
GetProjectionMatrix()    - 투영 행렬
SetPerspective(fov, ...)- 카메라 설정
```

### LightComponent

조명 정보

```
Color (glm::vec3)        - 조명 색상
Intensity (float)        - 밝기
```

## GameObject-Component 패턴

```
GameObject
└── Component[]
    ├── Transform
    ├── MeshRenderer
    ├── CameraComponent
    └── [Custom Component...]
```

**특징**:
- 템플릿 메서드로 타입 안전성
- 동적 캐스팅으로 런타임 다형성
- 자유로운 조합으로 기능 확장

## 데이터 흐름

```
Scene::OnUpdateEditor()
├── 각 GameObject 순회
│   └── GameObject::Update(dt)
│       └── 각 Component::OnUpdate(dt)
│
└── Rendering
    ├── Framebuffer Bind
    ├── 각 GameObject 렌더링
    │   ├── Transform 조회
    │   ├── MeshRenderer 조회
    │   └── Renderer::Submit(mesh, shader)
    └── ImGui Viewport 표시
```

## 설계 특징

1. **GameObject-Component** - 조합 기반 설계
2. **고유 ID** - GameObject 안정적 참조
3. **자동 생명주기** - unique_ptr로 자동 정리
4. **에디터/런타임 분리** - 두 가지 업데이트 모드

## 확장 포인트

- Component 상속 - 새로운 기능 추가
- Scene 확장 - 커스텀 업데이트 로직
- GameObject 메타데이터 - 이름, 태그 등
