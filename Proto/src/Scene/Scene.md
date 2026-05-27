# Scene Layer

Scene 계층은 게임 세계의 구성 요소들을 관리하며, **GameObject-Component 아키텍처**를 기반으로 객체의 생명주기, 상태 데이터, 그리고 런타임 로직의 흐름을 제어합니다.

## 📂 파일 구조
- `Scene.h/cpp`: 객체 컨테이너 및 시스템 업데이트 로직
- `GameObject.h/cpp`: 컴포넌트를 소유하는 엔티티 객체
- `Component.h`: 모든 컴포넌트의 기본 추상 클래스
- `SceneSerializer.h/cpp`: YAML 기반 씬 데이터 직렬화 및 역직렬화
- `ScriptRegistry.h/cpp`: 이름 기반 네이티브 스크립트 자동 등록 및 바인딩
- `Components/`: 내장 컴포넌트 라이브러리 (Transform, Mesh, Physics, Script 등)

---

## 🛠 핵심 시스템 상세

### 1. Scene
게임 세계의 최상위 관리자로, 모든 GameObject의 생성, 파괴 및 업데이트를 관장합니다.
- **Life Cycle**: `OnUpdateEditor`(에디터용 렌더링/업데이트)와 `OnUpdateRuntime`(물리/스크립트 로직 포함)으로 구분된 실행 흐름을 가집니다.
- **Physics Integration**: 런타임 업데이트 시 중력(`GRAVITY = -9.81f`), Rigidbody, 충돌 감지 및 분리(Collision Resolution) 연산을 수행합니다.
- **Grid Rendering**: 에디터 모드에서 `RenderGrid`를 통해 GLSL 기반 인피니트 그리드를 씬 뷰에 렌더링합니다.
- **Runtime Camera**: 런타임 렌더링은 Primary 카메라를 우선 사용하며, 없을 경우 첫 번째 CameraComponent로 대체합니다.
- **State Management**: 씬 내부의 모든 객체 상태를 관리하며, `SceneSerializer`를 통해 상태를 보존하거나 복구합니다.
- **Dirty Flag**: 씬 수정 여부(`SetDirty`, `IsDirty`)를 추적하여 저장 필요성 판단.
- **GameObject Creation**: `CreateGameObject(name)` (빈 객체)와 `CreateMeshGameObject(name, meshUUID)` (메쉬 포함 객체)를 통해 객체를 생성합니다.
  - 객체 생성/삭제 시 자동으로 `SetDirty(true)` 호출

### 2. GameObject
ID와 이름을 가진 데이터 컨테이너입니다. 실제 동작은 부착된 컴포넌트들에 의해 결정됩니다.
- **Component Management**: `AddComponent<T>`, `GetComponent<T>`, `HasComponent<T>` 템플릿 메서드를 통해 동적으로 기능을 확장하고 쿼리할 수 있습니다.
- **UUID**: 모든 객체는 생성 시 전역적으로 고유한 UUID를 부여받아 직렬화 및 에셋 참조에 사용됩니다.
- **RuntimeID**: 에디터의 오브젝트 피킹(Pixel Perfect Selection)을 위한 정수형 고유 ID를 별도로 관리합니다.

### 3. Component Interface
객체의 특정 기능을 정의하는 단위입니다.
- **Life Cycle Callbacks**: `OnStart`, `OnUpdate(dt)`, `OnDestroy`를 통해 시점별 로직을 구현합니다.
- **Persistence**: `Serialize`, `Deserialize` 메서드를 통해 자신의 데이터를 저장하고 로드할 책임을 가집니다.

### 4. SceneSerializer
YAML 포맷을 사용하여 씬 전체의 상태를 파일에 저장하거나 복원합니다.
- **UUID 기반 참조**: 에셋 연결 시 이름이 아닌 UUID를 사용하여 데이터 무결성을 보장합니다.
- **전체 컴포넌트 지원**: `Transform`, `MeshRenderer`, `CameraComponent`, `LightComponent`, `Rigidbody`, `BoxCollider`, `SphereCollider`, `NativeScriptComponent` 역직렬화를 지원합니다.
- **기본 메쉬 복원**: `MeshRenderer`는 기본 프리미티브 에셋 UUID와 메쉬 타입 이름을 서로 보완적으로 복원합니다.
- **스크립트 복원**: `NativeScriptComponent` 역직렬화 시 `ScriptRegistry::BindByName`을 통해 스크립트 이름을 읽고 실제 인스턴스를 자동으로 바인딩합니다.

---

## 🧩 내장 컴포넌트 카테고리

### 🔹 기본 및 렌더링 (Core & Rendering)
- **Transform**: 객체의 위치(Translation), 회전(Rotation), 크기(Scale)를 관리하며 오일러-쿼터니언 변환으로 월드 행렬을 계산합니다.
- **MeshRenderer**: 3D 메시 데이터(VertexArray)와 셰이더를 보관하며 렌더러에 그리기 정보를 제공합니다.
- **CameraComponent**: 씬을 투영할 시점과 파라미터(FOV, Near, Far)를 정의합니다. Primary 카메라가 런타임 렌더링에 사용됩니다.
- **LightComponent**: 방향성 광원(Directional)의 색상(Color)과 강도(Intensity)를 정의합니다.

### 🔹 물리 시스템 (Physics)
- **Rigidbody**: 질량(Mass), 속도(Velocity), 중력(UseGravity), 공기 저항(Drag)을 포함한 물리 동역학을 처리합니다.
- **Collider**: 충돌 영역의 추상 기반 클래스. 오프셋(Offset)과 트리거 여부(IsTrigger)를 공통으로 관리합니다.
- **BoxCollider**: 직육면체 형태의 충돌 영역. AABB 알고리즘으로 충돌을 감지합니다.
- **SphereCollider**: 구형 충돌 영역. 거리 기반 알고리즘으로 충돌을 감지합니다.

### 🔹 스크립팅 (Scripting)
- **NativeScriptComponent**: C++ 클래스(`ScriptableEntity` 상속)를 통해 사용자 정의 로직을 추가합니다. `OnStart`, `OnUpdate`, `OnDestroy`, `OnCollisionEnter` 콜백을 지원합니다.
- **ScriptRegistry**: 등록된 스크립트 클래스를 이름(문자열)과 매핑하여, 에디터에서 선택하거나 씬 파일에서 로드할 때 동적으로 스크립트를 생성 및 바인딩합니다.

---

## 🎨 디자인 원칙
*   **Composition over Inheritance**: 상속을 통한 기능 확장이 아닌, 컴포넌트 조합을 통해 객체의 기능을 정의합니다.
*   **Minimal Coupling**: 컴포넌트들은 서로의 존재를 최소한으로 알며, 필요 시 `GetComponent`를 통해 통신합니다.
*   **Editor-Friendly**: 모든 데이터는 직렬화 가능해야 하며, 이는 에디터의 Inspector 패널에서 실시간으로 수정 가능함을 의미합니다.
*   **한국어 로그**: 스크립트 바인딩 실패, 직렬화 오류 등 씬 관련 에러 로그는 한국어로 출력합니다.
